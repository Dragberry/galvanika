from datetime import datetime
from io import BytesIO
import uuid

import PIL
from django.core.files.base import ContentFile, File
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
    no_index: bool = models.BooleanField(default=False)
    deleted: bool = models.BooleanField(default=False)

    class Meta:
        abstract = True


class Image(models.Model):
    id: int = models.AutoField(primary_key=True)
    title: str = models.CharField(max_length=256)
    folder: str = models.CharField(max_length=32, default='common')
    original = models.ImageField(upload_to='images', blank=True, null=True)
    preview = models.ImageField(upload_to='images', blank=True, null=True)
    thumbnail = models.ImageField(upload_to='images', blank=True, null=True)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def save(self, **kwargs):
        template: str = f'{self.folder}/{slugify(self.title)}-{uuid.uuid4().hex[:8].upper()}-%s.jpg'
        try:
            im = PIL.Image.open(self.original)
            source_image = im.convert('RGB')
            output = BytesIO()
            source_image.save(output, format='JPEG')
            output.seek(0)
            content_file = ContentFile(output.read())
            self.original.save(template % 'original', content_file, save=False)

            source_image = im.convert('RGB')
            source_image.thumbnail((200, 200))
            output = BytesIO()
            source_image.save(output, format='JPEG')
            output.seek(0)
            content_file = ContentFile(output.read())
            file = File(content_file)
            self.thumbnail.save(template % 'thumbnail', file, save=False)

            source_image = im.convert('RGB')
            source_image.thumbnail((640, 480))
            output = BytesIO()
            source_image.save(output, format='JPEG')
            output.seek(0)
            content_file = ContentFile(output.read())
            file = File(content_file)
            self.preview.save(template % 'preview', file, save=False)
        except ValueError:
            if self.original is not None:
                self.original.delete()
                self.original = None
            if self.thumbnail is not None:
                self.thumbnail.delete()
                self.thumbnail = None
            if self.preview is not None:
                self.preview.delete()
                self.preview = None
        super().save(**kwargs)

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
    id: int = models.AutoField(primary_key=True)
    name: str = models.CharField(max_length=36)
    short_description: str = models.CharField(max_length=1024, blank=True, null=True)
    long_description: str = models.CharField(max_length=4096, blank=True, null=True)
    price = models.DecimalField(max_digits=10, decimal_places=2, blank=True, null=True)
    actual_price = models.DecimalField(max_digits=10, decimal_places=2, blank=True, null=True)
    sold: bool = models.BooleanField(default=False)
    categories: [Category] = models.ManyToManyField(Category)
    main_image: Image = models.OneToOneField(Image, related_name='main_image', on_delete=models.CASCADE, blank=True, null=True)
    images: [Image] = models.ManyToManyField(Image)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def url_name(self) -> str:
        return f'{slugify(self.name, "ru")}-{self.id}' if not self.url else self.url[self.url.rfind('/') + 1:]

    def __str__(self):
        return f'Product({self.id}, {self.name}{", DELETED" if self.deleted else ""})'


class OrderItem(models.Model):
    id: int = models.AutoField(primary_key=True)
    product: Product = models.ForeignKey(Product, on_delete=models.PROTECT)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def __str__(self):
        return f'OrderItem(id={self.pk}, product_id={self.product.id}, product_name={self.product.name})'


class Order(models.Model):
    id: int = models.AutoField(primary_key=True)
    total_amount = models.DecimalField(max_digits=10, decimal_places=2)
    name: str = models.CharField(max_length=36, blank=True, null=True)
    mobile: str = models.CharField(max_length=36, blank=True, null=True)
    email: str = models.CharField(max_length=36, blank=True, null=True)
    address: str = models.CharField(max_length=256, blank=True, null=True)
    comment: str = models.CharField(max_length=128, blank=True, null=True)
    items: [OrderItem] = models.ManyToManyField(OrderItem)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)

    def __str__(self):
        return f'Order(id={self.pk}, total_amount={self.total_amount})'


class QuickReferenceCard(models.Model):
    id: int = models.AutoField(primary_key=True)
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
    id: int = models.AutoField(primary_key=True)
    title: str = models.CharField(max_length=256)
    content: str = models.CharField(max_length=4096)
    created_datetime: datetime = models.DateTimeField(default=now)
    created_by_id: int = models.IntegerField(default=-1)
    product: Product = models.OneToOneField(Product, on_delete=models.PROTECT, blank=True, null=True)
