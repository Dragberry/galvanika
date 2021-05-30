from django.conf.urls import url
from django.urls import path

from . import views
from .views import CatalogView, ProductView, Cart

app_name: str = 'core'

urlpatterns = [
    path('', views.index, name='index'),
    path('informaciya', views.information, name='information'),
    url('^load-more-posts/(?P<offset>\\d+)$', views.load_more_posts, name='load_more_posts'),
    url('^catalog/quick-order$', views.catalog_quick_order, name='quick_order'),
    url('^catalog/(?P<category_ids>[a-zA-Z0-9\\-/]*)/(?:(?P<url>.+)-(?P<pk>\\d+)?)$', ProductView.as_view(), name='product'),
    url('^catalog/(?:(?P<url>.+)-(?P<pk>\\d+)?)$', ProductView.as_view()),
    url('^catalog/(?P<category_ids>[a-zA-Z0-9\\-/]+)$', CatalogView.as_view(), name='category'),
    url('^catalog$', CatalogView.as_view(), name='catalog'),
    url('^cart$', Cart.index, name='cart'),
    url('^cart/add$', Cart.add_item, name='cart_add_item'),
    url('^cart/remove', Cart.remove_item, name='cart_remove_item'),
    url('^cart/next', Cart.next, name='cart_go_next'),
    url('^cart/back', Cart.back, name='cart_go_back'),
    url('^cart/submit-order', Cart.submit_order, name='cart_submit_order'),
]
