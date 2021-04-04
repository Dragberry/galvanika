from dataclasses import dataclass, field
from enum import Enum
from typing import Optional

from django.db.models import Case, When, Q
from django.http import HttpResponse, HttpResponseRedirect
from django.shortcuts import render, get_object_or_404
from django.template import loader
from django.urls import reverse
from django.views.generic import ListView, DetailView

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


@dataclass
class CatalogFilters:
    sort: SortEnum = SortEnum.DATE_DESC
    display: DisplayEnum = DisplayEnum.TILES
    page_size: int = 4
    bases: [str] = field(default_factory=list)


class CatalogView(ListView):
    template_name = 'catalog/catalog.html'
    context_object_name = 'products'

    page_sizes: [int] = [4, 24, 48]

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.catalog_filters: Optional[dict] = None
        self.categories: [Category] = []

    def setup(self, request, *args, **kwargs):
        super().setup(request, *args, **kwargs)
        self.catalog_filters: [str, dict] = request.session.setdefault('catalog_filters', {
            'sort': SortEnum.DATE_DESC.value,
            'display': DisplayEnum.TILES.value,
            'page_size': CatalogView.page_sizes[0],
        })

    def get_paginate_by(self, queryset):
        try:
            page_size: int = int(self.request.GET['pageSize'])
            if page_size not in CatalogView.page_sizes:
                raise ValueError()
            self.catalog_filters['page_size'] = page_size
        except (KeyError, ValueError):
            if 'page_size' not in self.catalog_filters:
                self.catalog_filters['page_size'] = CatalogView.page_sizes[0]
        return self.catalog_filters['page_size']

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
            self.catalog_filters['sort'] = sort.name
        except KeyError:
            sort: SortEnum = SortEnum[self.catalog_filters['sort']]

        filters: [Q] = []

        category_ids: str = self.kwargs.get('category_ids')
        if category_ids is not None:
            category_ids_list: [str] = category_ids.split('/')
            for category_id in category_ids_list:
                self.categories.append(get_object_or_404(Category, pk=category_id))
            filters.append(Q(categories__id=category_ids_list[-1]))

        return Product.objects.annotate(
            real_price=Case(
                When(actual_price__isnull=False, then='actual_price'),
                default='price',
            )
        ).filter(*filters).order_by(sort.value)

    def get_context_data(self, **kwargs):
        context: dict = super(CatalogView, self).get_context_data(**kwargs)

        context['page_sizes'] = CatalogView.page_sizes

        if self.categories:
            context['categories'] = self.categories
            context['parent_category'] = self.categories[0]
            context['current_category'] = self.categories[-1]
        context['sorts'] = {sort.name: sort.value for sort in SortEnum}

        try:
            display: DisplayEnum = DisplayEnum(self.request.GET.get('display'))
            self.catalog_filters['display'] = display.value
        except (KeyError, ValueError):
            if 'display' not in self.catalog_filters:
                self.catalog_filters['display'] = DisplayEnum.TILES.value

        context['catalog_filters'] = self.catalog_filters
        self.request.session['catalog_filters'] = self.catalog_filters
        return context


class ProductView(DetailView):
    template_name = 'catalog/product/product.html'
    context_object_name = 'product'
    model = Product

    def get_context_data(self, **kwargs):
        context: dict = super(ProductView, self).get_context_data(**kwargs)
        categories: [Category] = []
        category_ids: str = self.kwargs.get('category_ids')
        if category_ids is not None:
            for category_id in category_ids.split('/'):
                categories.append(get_object_or_404(Category, pk=category_id))
        context['categories'] = categories
        context['images'] = self.object.productimage_set.order_by('order')
        return context


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
