from django.contrib import admin

from .models import Product, ProductOrder, ProductImage, BlogPost, Category, Social

admin.site.register(Social)
admin.site.register(Category)
admin.site.register(Product)
admin.site.register(ProductOrder)
admin.site.register(ProductImage)
admin.site.register(BlogPost)
