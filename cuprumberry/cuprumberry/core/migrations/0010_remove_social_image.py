# Generated by Django 3.1.7 on 2021-03-26 22:53

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('core', '0009_auto_20210327_0150'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='social',
            name='image',
        ),
    ]