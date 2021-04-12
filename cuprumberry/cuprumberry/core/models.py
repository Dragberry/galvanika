from datetime import datetime

from django.db import models
from django.utils.timezone import now
from transliterate import slugify


class Social(models.Model):
    name: str = models.CharField(max_length=32)
    action: str = models.CharField(max_length=64)


class SeoPage(models.Model):
    url: str = models.CharField(max_length=255, blank=True, null=True)
    page_title: str = models.CharField(max_length=255, blank=True, null=True)
    page_description: str = models.CharField(max_length=1024, blank=True, null=True)
    page_keywords: str = models.CharField(max_length=1024, blank=True, null=True)
    page_h1: str = models.CharField(max_length=128, blank=True, null=True)
    deleted: bool = models.BooleanField(default=False)

    class Meta:
        abstract = True


class Image(models.Model):
    id: int = models.IntegerField(primary_key=True)
    title: str = models.CharField(max_length=256)
    image = models.ImageField(upload_to='images')
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def __str__(self):
        return f'Image({self.id}, {self.title})'


class Category(SeoPage):
    id: str = models.CharField(max_length=32, primary_key=True)
    name: str = models.CharField(max_length=32)
    parent = models.ForeignKey("self", on_delete=models.PROTECT, blank=True, null=True)
    description = models.CharField(max_length=4096, blank=True, null=True)
    image: Image = models.OneToOneField(Image, on_delete=models.CASCADE, blank=True, null=True)

    def full_path(self) -> str:
        categories = []
        category = self
        while category is not None:
            categories.append(category)
            category = category.parent

        return '/'.join([c.id for c in reversed(categories)])

    def __str__(self):
        return f'Category({self.id}, {self.name}{", DELETED" if self.deleted else ""})'


class Product(SeoPage):
    id: int = models.IntegerField(primary_key=True)
    name: str = models.CharField(max_length=36)
    short_description: str = models.CharField(max_length=1024, blank=True, null=True)
    long_description: str = models.CharField(max_length=4096, blank=True, null=True)
    price = models.DecimalField(max_digits=10, decimal_places=2, blank=True, null=True)
    actual_price = models.DecimalField(max_digits=10, decimal_places=2, blank=True, null=True)
    categories: [Category] = models.ManyToManyField(Category)
    main_image: Image = models.OneToOneField(Image, related_name='main_image', on_delete=models.CASCADE, blank=True, null=True)
    images: [Image] = models.ManyToManyField(Image)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def url_name(self) -> str:
        return self.url if self.url is not None else slugify(self.name, 'ru')

    def __str__(self):
        return f'Product({self.id}, {self.name}{", DELETED" if self.deleted else ""})'


class ProductOrder(models.Model):
    product: Product = models.ForeignKey(Product, on_delete=models.PROTECT)
    mobile: str = models.CharField(max_length=36, blank=True, null=True)
    email: str = models.CharField(max_length=36, blank=True, null=True)
    address: str = models.CharField(max_length=256, blank=True, null=True)
    comment: str = models.CharField(max_length=128, blank=True, null=True)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def __str__(self):
        return f'ProductOrder(id={self.pk}, product_id={self.product.name}, product_id={self.prвoduct.name})'


class QuickReferenceCard(models.Model):
    title: str = models.CharField(max_length=256)
    content: str = models.CharField(max_length=4096)
    order: bool = models.IntegerField(default=0)
    deleted: bool = models.BooleanField(default=False)
    product: Product = models.OneToOneField(Product, on_delete=models.PROTECT, blank=True, null=True)
    category: Category = models.OneToOneField(Category, on_delete=models.PROTECT, blank=True, null=True)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def __str__(self):
        return f'QuickReferenceCard({self.pk}, {self.title})'


class BlogPost(models.Model):
    title: str = models.CharField(max_length=256)
    content: str = models.CharField(max_length=4096)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)
    product: Product = models.OneToOneField(Product, on_delete=models.PROTECT, blank=True, null=True)
