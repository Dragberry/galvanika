# Generated by Django 3.1.7 on 2021-03-21 21:42

import datetime
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('core', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='product',
            name='created_by_id',
            field=models.IntegerField(default=-1),
        ),
        migrations.AlterField(
            model_name='product',
            name='created_datetime',
            field=models.DateTimeField(default=datetime.datetime(2021, 3, 22, 0, 42, 31, 397488)),
        ),
        migrations.CreateModel(
            name='ProductOrder',
            fields=[
                ('id', models.IntegerField(primary_key=True, serialize=False)),
                ('mobile', models.CharField(max_length=36)),
                ('email', models.CharField(max_length=36)),
                ('created_datetime', models.DateTimeField(default=datetime.datetime(2021, 3, 22, 0, 42, 31, 398036))),
                ('created_by_id', models.IntegerField(default=-1)),
                ('product', models.ForeignKey(on_delete=django.db.models.deletion.PROTECT, to='core.product')),
            ],
        ),
    ]