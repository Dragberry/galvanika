from enum import Enum
from typing import Optional

from django.db.models import Case, When, Value
from django.http import HttpResponse, HttpResponseRedirect
from django.shortcuts import render, get_object_or_404
from django.template import loader
from django.urls import reverse

from .models import Product, ProductOrder, ProductImage


class DisplayEnum(Enum):
    TILES = 'table'
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


def catalog_category(request, category_id: Optional[int], category_name: Optional[str]):
    return catalog(request)


def catalog(request):
    display: str = request.GET.get('display')
    if 'display' not in request.session:
        request.session['display'] = 'tiles' if display is None else display
    elif display is not None:
        request.session['display'] = display

    try:
        sort: SortEnum = SortEnum[request.GET.get('sort')]
    except KeyError:
        sort: SortEnum = SortEnum.DATE_DESC
    request.session['sort'] = sort.value

    page: int = int(request.GET.get('page', '1'))
    page_size: int = 2
    total: int = Product.objects.count()
    total_pages: int = (total - 1) // page_size + 1
    products: [Product] = Product.objects.annotate(
        real_price=Case(
            When(actual_price__isnull=False, then='actual_price'),
            default='price',
        )
    ).order_by(sort.value)[:5]
    context = {
        'sorts': {sort.name: sort.value for sort in SortEnum},
        'products': {
            product: ProductImage.objects.get(product=product, order=0) for product in products
        },
        'page': page,
        'total_pages': total_pages,
        'pages': range(1, total_pages + 1)
    }
    if request.is_ajax():
        return render(request, 'catalog/product-list.html', context)
    return render(request, 'catalog/catalog.html', context)


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
