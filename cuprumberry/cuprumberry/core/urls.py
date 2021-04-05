from django.conf.urls import url
from django.urls import path
from django.views.generic import RedirectView

from . import views
from .views import CatalogView, ProductView

app_name: str = 'core'

urlpatterns = [
    path('index', views.index, name='index'),
    path('', RedirectView.as_view(url='index')),
    url('^catalog/quick-order$', views.catalog_quick_order, name='quick_order'),
    url('^catalog/(?P<category_ids>[a-zA-Z0-9\\-/]*)/(?:(?P<url>.+)-(?P<pk>\\d+)?)$', ProductView.as_view()),
    url('^catalog/(?:(?P<url>.+)-(?P<pk>\\d+)?)$', ProductView.as_view()),
    url('^catalog/(?P<category_ids>[a-zA-Z0-9\\-/]+)$', CatalogView.as_view(), name='category'),
    url('^catalog$', CatalogView.as_view(), name='catalog'),
]
