# Generated by Django 3.1.7 on 2021-04-29 19:26

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('core', '0006_auto_20210429_2152'),
    ]

    operations = [
        migrations.RenameField(
            model_name='image',
            old_name='image',
            new_name='original',
        ),
        migrations.RemoveField(
            model_name='image',
            name='source',
        ),
    ]
