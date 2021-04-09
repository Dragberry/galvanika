from django.contrib import admin

from .models import Product, ProductOrder, Image, BlogPost, Category, Social

admin.site.register(Social)
admin.site.register(Category)
admin.site.register(Product)
admin.site.register(ProductOrder)
admin.site.register(Image)
admin.site.register(BlogPost)
