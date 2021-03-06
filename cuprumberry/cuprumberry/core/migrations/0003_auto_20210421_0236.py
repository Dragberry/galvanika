# Generated by Django 3.1.7 on 2021-04-20 23:36

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('core', '0002_product_sold'),
    ]

    operations = [
        migrations.AddField(
            model_name='image',
            name='preview',
            field=models.ImageField(blank=True, null=True, upload_to='images'),
        ),
        migrations.AddField(
            model_name='image',
            name='thumbnail',
            field=models.ImageField(blank=True, null=True, upload_to='images'),
        ),
    ]
