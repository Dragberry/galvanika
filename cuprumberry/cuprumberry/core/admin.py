from django.contrib import admin

from .models import Order, OrderItem, Product, Image, BlogPost, Category, Social, QuickReferenceCard

admin.site.register(Social)
admin.site.register(Category)
admin.site.register(Order)
admin.site.register(OrderItem)
admin.site.register(Product)
admin.site.register(Image)
admin.site.register(BlogPost)
admin.site.register(QuickReferenceCard)
