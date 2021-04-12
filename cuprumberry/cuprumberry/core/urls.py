from django.conf.urls import url
from django.urls import path

from . import views
from .views import CatalogView, ProductView

app_name: str = 'core'

urlpatterns = [
    path('', views.index, name='index'),
    path('dostavka-i-oplata', views.delivery_and_payment, name='delivery_and_payment'),
    path('contacty', views.contacts, name='contacts'),
    url('^load-more-posts/(?P<offset>\\d+)$', views.load_more_posts, name='load_more_posts'),
    url('^catalog/quick-order$', views.catalog_quick_order, name='quick_order'),
    url('^catalog/(?P<category_ids>[a-zA-Z0-9\\-/]*)/(?:(?P<url>.+)-(?P<pk>\\d+)?)$', ProductView.as_view()),
    url('^catalog/(?:(?P<url>.+)-(?P<pk>\\d+)?)$', ProductView.as_view()),
    url('^catalog/(?P<category_ids>[a-zA-Z0-9\\-/]+)$', CatalogView.as_view(), name='category'),
    url('^catalog$', CatalogView.as_view(), name='catalog'),
]
