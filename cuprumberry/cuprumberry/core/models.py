from datetime import datetime

from django.db import models
from django.utils.timezone import now
from transliterate import translit, slugify


class Social(models.Model):
    name: str = models.CharField(max_length=32)
    action: str = models.CharField(max_length=64)


class Category(models.Model):
    id: str = models.CharField(max_length=32, primary_key=True)
    name: str = models.CharField(max_length=32)
    parent = models.ForeignKey("self", on_delete=models.PROTECT, null=True)

    def __str__(self):
        return f'Category({self.name})'


class Product(models.Model):
    id: int = models.IntegerField(primary_key=True)
    name: str = models.CharField(max_length=36)
    short_description: str = models.CharField(max_length=1024, null=True)
    long_description: str = models.CharField(max_length=4096, null=True)
    price = models.DecimalField(max_digits=10, decimal_places=2, null=True)
    actual_price = models.DecimalField(max_digits=10, decimal_places=2, null=True)
    categories: [Category] = models.ManyToManyField(Category)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def url_name(self) -> str:
        return slugify(self.name, 'ru')

    def main_image(self):
        return ProductImage.objects.get(product=self, order=0)

    def __str__(self):
        return f'Product(id={self.id}, name={self.name})'


class ProductOrder(models.Model):
    product: Product = models.ForeignKey(Product, on_delete=models.PROTECT)
    mobile: str = models.CharField(max_length=36, null=True)
    email: str = models.CharField(max_length=36, null=True)
    address: str = models.CharField(max_length=256, null=True)
    comment: str = models.CharField(max_length=128, null=True)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def __str__(self):
        return f'ProductOrder(id={self.id}, product_id={self.product.name}, product_id={self.product.name})'


class ProductImage(models.Model):
    id: int = models.IntegerField(primary_key=True)
    product: Product = models.ForeignKey(Product, on_delete=models.CASCADE)
    title: str = models.CharField(max_length=256)
    order: int = models.IntegerField(default=0)
    image = models.ImageField(upload_to='images')
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def __str__(self):
        return f'ProductImage({self.title})'


class BlogPost(models.Model):
    id: int = models.IntegerField(primary_key=True)
    title: str = models.CharField(max_length=256)
    content: str = models.CharField(max_length=4096)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)
