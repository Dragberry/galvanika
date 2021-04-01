from django.urls import path
from django.views.generic import RedirectView

from . import views
from .views import CatalogView

app_name: str = 'core'


urlpatterns = [
    path('index', views.index, name='index'),
    path('', RedirectView.as_view(url='index')),
    path('catalog', CatalogView.as_view(), name='catalog'),
    path('catalog/categories/<int:category_id>/<str:category_name>', CatalogView.as_view(), name='catalog_category'),
    path('products/<int:product_id>/', views.product_detail, name='product_detail'),
    path('products/<int:product_id>/order', views.quick_order, name='quick_order'),

]
