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
    products: [Product] = Product.objects.order_by('-created_datetime')[:5]

    template = loader.get_template('catalog/catalog.html')
    context = {
        'products': {
            product: ProductImage.objects.get(product=product, order=0) for product in products
        },
    }
    return HttpResponse(template.render(context, request))


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
