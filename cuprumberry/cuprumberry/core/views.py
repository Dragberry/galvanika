from django.http import HttpResponse, HttpResponseRedirect
from django.shortcuts import render, get_object_or_404
from django.template import loader
from django.urls import reverse

from .models import Product, ProductOrder, ProductImage


def index(request):
    products: [Product] = Product.objects.order_by('-created_datetime')[:5]
    template = loader.get_template('index.html')
    context = {
        'product_list': products,
    }
    return HttpResponse(template.render(context, request))


def catalog(request):
    display: str = request.GET.get('display')
    if 'display' not in request.session:
        request.session['display'] = 'tiles' if display is None else display
    elif display is not None:
        request.session['display'] = display

    products: [Product] = Product.objects.order_by('-created_datetime')[:5]
    context = {
        'products': {
            product: ProductImage.objects.get(product=product, order=0) for product in products
        },
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
