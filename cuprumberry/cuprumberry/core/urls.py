from django.urls import path
from django.views.generic import RedirectView

from . import views

app_name: str = 'core'


urlpatterns = [
    path('index', views.index, name='index'),
    path('', RedirectView.as_view(url='index')),
    path('catalog', views.catalog, name='catalog'),
    path('products/<int:product_id>/', views.product_detail, name='product_detail'),
    path('products/<int:product_id>/order', views.quick_order, name='quick_order'),

]
