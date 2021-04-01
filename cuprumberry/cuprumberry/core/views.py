from enum import Enum

from django.db.models import Case, When
from django.http import HttpResponse, HttpResponseRedirect
from django.shortcuts import render, get_object_or_404
from django.template import loader
from django.urls import reverse
from django.views.generic import ListView

from .models import Product, ProductOrder, Category


class DisplayEnum(Enum):
    TILES = 'tiles'
    LIST = 'list'


class SortEnum(Enum):
    DATE_DESC = '-created_datetime'
    DATE_ASC = 'created_datetime'
    PRICE_DESC = '-real_price'
    PRICE_ASC = 'real_price'


def index(request):
    products: [Product] = Product.objects.order_by('-created_datetime')[:5]
    template = loader.get_template('index.html')
    context = {
        'product_list': products,
    }
    return HttpResponse(template.render(context, request))


class CatalogView(ListView):
    template_name = 'catalog/catalog.html'
    context_object_name = 'products'
    paginate_by = 2

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.categories: [Category] = []

    def render_to_response(self, context, **response_kwargs):
        if self.request.is_ajax():
            response_kwargs.setdefault('content_type', self.content_type)
            return self.response_class(
                request=self.request,
                template='catalog/product-list.html',
                context=context,
                using=self.template_engine,
                **response_kwargs
            )
        return super().render_to_response(context, **response_kwargs)

    def get_queryset(self):
        try:
            sort: SortEnum = SortEnum[self.request.GET.get('sort')]
        except KeyError:
            sort: SortEnum = SortEnum.DATE_DESC
        self.request.session['sort'] = sort.value

        category_id: int = self.kwargs.get('category_id')
        if category_id is not None:
            category: Category = get_object_or_404(Category, pk=category_id)
            while category.parent is not None:
                self.categories.append(category)
                category = category.parent
            self.categories.append(category)
            self.categories.reverse()

        filters: dict = {}
        if category_id is not None:
            filters['categories__id__in'] = [category_id]
        return Product.objects.annotate(
            real_price=Case(
                When(actual_price__isnull=False, then='actual_price'),
                default='price',
            )
        ).filter(**filters).order_by(sort.value)

    def get_context_data(self, **kwargs):
        context: dict = super(CatalogView, self).get_context_data(**kwargs)
        if self.categories:
            context['categories'] = self.categories
        context['sorts'] = {sort.name: sort.value for sort in SortEnum}

        try:
            display: DisplayEnum = DisplayEnum(self.request.GET.get('display'))
            self.request.session['display'] = display.value
        except (KeyError, ValueError):
            if 'display' not in self.request.session:
                self.request.session['display'] = DisplayEnum.TILES.value
        return context


def product_detail(request, product_id: str):
    product: Product = get_object_or_404(Product, pk=product_id)
    return render(request, 'products/product_detail.html', {'product': product})


def quick_order(request, product_id: str):
    product: Product = get_object_or_404(Product, pk=product_id)
    mobile: str = request.POST['mobile']
    email: str = request.POST['email']
    if not mobile or not email:
        return render(request, 'products/product_detail.html', {
            'product': product,
            'error_message': "Please, specify your contact details.",
        })
    else:
        product_order: ProductOrder = ProductOrder(
            product=product,
            mobile=mobile,
            email=email
        )
        product_order.save()
        return HttpResponseRedirect(reverse('core:product_detail', args=(product.id,)))
