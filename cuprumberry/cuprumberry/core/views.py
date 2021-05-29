from dataclasses import dataclass, field
from enum import Enum
from typing import Optional, Set

from django.db.models import Case, When, Q
from django.http import HttpResponse, JsonResponse, HttpResponseBadRequest
from django.shortcuts import get_object_or_404
from django.template import loader
from django.urls import reverse
from django.utils.translation import gettext
from django.views.generic import ListView, DetailView

from .models import Product, ProductOrder, Category, BlogPost, QuickReferenceCard


class DisplayEnum(Enum):
    TILES = 'tiles'
    LIST = 'list'


class SortEnum(Enum):
    DATE_DESC = '-created_datetime'
    DATE_ASC = 'created_datetime'
    PRICE_DESC = '-real_price'
    PRICE_ASC = 'real_price'


PAGE_SIZE: int = 5


def index(request):
    posts: [BlogPost] = BlogPost.objects.order_by('-created_datetime')[:PAGE_SIZE]
    page_size: int = len(posts)
    quick_references: [QuickReferenceCard] = QuickReferenceCard.objects.order_by('order')[:3]
    template = loader.get_template('index.html')
    context = {
        'posts': posts,
        'offset': page_size,
        'is_last_page': page_size < PAGE_SIZE,
        'quick_references': quick_references
    }
    return HttpResponse(template.render(context, request))


def load_more_posts(request, offset: str):
    if request.is_ajax():
        try:
            offset: int = int(offset)
        except ValueError:
            offset: int = 0
        posts: [BlogPost] = BlogPost.objects.order_by('-created_datetime')[offset:offset + PAGE_SIZE]
        page_size: int = len(posts)
        context = {
            'posts': posts,
            'offset': offset + page_size,
            'is_last_page': page_size < PAGE_SIZE,
        }
        template = loader.get_template('common/main/load-more-posts.html')
        return HttpResponse(template.render(context, request))
    return HttpResponseBadRequest()


@dataclass
class CatalogFilters:
    sort: SortEnum = SortEnum.DATE_DESC
    display: DisplayEnum = DisplayEnum.TILES
    page_size: int = 12
    bases: [str] = field(default_factory=list)


class CatalogView(ListView):
    template_name = 'common/catalog/catalog.html'
    context_object_name = 'products'

    page_sizes: [int] = [12, 24, 48]

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.catalog_filters: Optional[dict] = None
        self.categories: [Category] = []

    def setup(self, request, *args, **kwargs):
        super().setup(request, *args, **kwargs)
        self.catalog_filters: [str, dict] = request.session.setdefault('catalog_filters', {
            'sort': SortEnum.DATE_DESC.name,
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
                template='common/catalog/product-list.html',
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

        filters: [Q] = [Q(deleted=False), Q(sold=False)]

        category_ids: str = self.kwargs.get('category_ids')
        if category_ids is not None:
            category_ids_list: [str] = category_ids.split('/')
            for category_id in category_ids_list:
                self.categories.append(get_object_or_404(Category, Q(pk=category_id) & Q(deleted=False)))
            filters.append(Q(categories__id=category_ids_list[-1]))
        else:
            self.categories.append(Category.objects.get(pk='all'))

        return Product.objects.annotate(
            real_price=Case(
                When(actual_price__isnull=False, then='actual_price'),
                default='price',
            )
        ).filter(*filters).order_by(sort.value)

    def get_context_data(self, **kwargs):
        context: dict = super(CatalogView, self).get_context_data(**kwargs)

        context['page_sizes'] = CatalogView.page_sizes

        breadcrumb: [str, str] = {
            gettext('home'): reverse('core:index'),
            gettext('catalog'): reverse('core:catalog'),
        }
        if self.categories:
            context['categories'] = self.categories
            context['parent_category'] = self.categories[0]
            context['current_category'] = self.categories[-1]
            full_category_id: str = ''
            for category in self.categories:
                full_category_id = f'{full_category_id}/{category.id}' if full_category_id else category.id
                breadcrumb[category.name] = reverse('core:category', kwargs={'category_ids': full_category_id})
        context['breadcrumb'] = breadcrumb

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
    template_name = 'common/catalog/product/product.html'
    context_object_name = 'product'
    model = Product

    def get_context_data(self, **kwargs):
        context: dict = super(ProductView, self).get_context_data(**kwargs)
        breadcrumb: [str, str] = {
            gettext('home'): reverse('core:index'),
            gettext('catalog'): reverse('core:catalog'),
        }
        categories: [Category] = []
        category_ids: str = self.kwargs.get('category_ids')
        if category_ids is not None:
            full_category_id: str = ''
            for category_id in category_ids.split('/'):
                category: Category = get_object_or_404(Category, pk=category_id)
                categories.append(category)
                full_category_id = f'{full_category_id}/{category.id}' if full_category_id else category.id
                breadcrumb[category.name] = reverse('core:category', kwargs={'category_ids': full_category_id})
        context['breadcrumb'] = breadcrumb
        context['categories'] = categories
        context['images'] = self.object.images.all()

        return context

    def get_queryset(self, queryset=None):
        qs = super().get_queryset()
        return qs.filter(Q(deleted=False) & Q(sold=False))


def catalog_quick_order(request):
    if request.is_ajax():
        fields: dict = {
            'productId': False,
            'mobile': False,
            'email': False,
            'address': False,
            'comment': False,
        }
        status: int = 200
        product_id: str = request.POST.get('productId')
        if not product_id:
            status = 404
            fields['productId'] = gettext('Quick order error: no productId')
        mobile: str = request.POST.get('mobile')
        email: str = request.POST.get('email')
        if not mobile and not email:
            status = 404
            fields['mobile'] = gettext('Quick order error: either mobile or email must be provided')
            fields['email'] = gettext('Quick order error: either mobile or email must be provided')
        address: str = request.POST.get('address')
        if not address:
            status = 404
            fields['address'] = gettext('Quick order error: no address')
        comment: str = request.POST.get('comment')
        if status == 200:
            try:
                product: Product = Product.objects.get(id=int(product_id))
                product_order: ProductOrder = ProductOrder(
                    product=product,
                    mobile=mobile,
                    email=email,
                    address=address,
                    comment=comment
                )
                product_order.save()
            except Product.DoesNotExist:
                status = 404
                fields['productId'] = gettext('Quick order error: no product')
            except Exception as e:
                print(e)
                status = 500

        return JsonResponse(data={'fields': fields}, status=status)
    return HttpResponseBadRequest()


def information(request):
    template = loader.get_template('common/information.html')
    return HttpResponse(template.render({}, request))


class CartStage(Enum):
    CART = 'CART'
    ORDER = 'ORDER'
    SUCCESS = 'SUCCESS'


class Cart:
    @staticmethod
    def index(request):
        try:
            cart_stage: CartStage = CartStage[request.session['cart_stage']]
        except KeyError:
            cart_stage: CartStage = CartStage.CART

        if cart_stage == CartStage.ORDER:
            template_name: str = 'common/cart/cart-order-details.html'
        elif cart_stage == CartStage.SUCCESS:
            template_name: str = 'common/cart/cart-order-success.html'
        else:
            template_name: str = 'common/cart/cart-product-list.html'

        products_in_cart: [str] = request.session.setdefault('cart', [])
        products: [Product] = [Product.objects.get(id=product_id) for product_id in products_in_cart]
        template = loader.get_template(template_name)
        context = {
            'products': products,
            'total_order_amount': sum([p.price if p.price else p.actual_price for p in products])
        }
        return HttpResponse(template.render(context, request))

    @staticmethod
    def add_item(request):
        if request.is_ajax():
            status: int = 200
            product_id: str = request.POST.get('productId')
            data: dict = {'productId': product_id}
            try:
                product: Product = get_object_or_404(Product, Q(id=int(product_id)) & Q(deleted=False) & Q(sold=False))
                data['productName'] = product.name
                data['productImageUrl'] = product.main_image.thumbnail.url
                products_in_cart: [str] = request.session.setdefault('cart', [])
                if product.id not in products_in_cart:
                    products_in_cart.append(product.id)
                    request.session['cart'] = products_in_cart
                    request.session['cart_product_count'] = len(products_in_cart)
                    data['cartProductCount'] = len(products_in_cart)
                else:
                    data['error'] = gettext('Cart error: Item is already in cart')
                    status = 409
            except ValueError:
                status = 400
                data['error'] = gettext('Error: Bad request')
            except Product.DoesNotExist:
                status = 404
                data['error'] = gettext('Cart error: Product not found')
            except Exception as e:
                print(e)
                status = 500
                data['error'] = gettext('Error: Internal server error')

            return JsonResponse(data=data, status=status)
        return HttpResponseBadRequest()

    @staticmethod
    def remove_item(request):
        pass

    @staticmethod
    def next(request):
        pass

    @staticmethod
    def submit_order(request):
        pass