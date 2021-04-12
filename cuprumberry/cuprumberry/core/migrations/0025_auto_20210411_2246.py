# Generated by Django 3.1.7 on 2021-04-11 19:46

from django.db import migrations, models
import django.db.models.deletion
import django.utils.timezone


class Migration(migrations.Migration):

    dependencies = [
        ('core', '0024_auto_20210410_0036'),
    ]

    operations = [
        migrations.AddField(
            model_name='category',
            name='image',
            field=models.OneToOneField(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to='core.image'),
        ),
        migrations.CreateModel(
            name='QuickReferenceCard',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('title', models.CharField(max_length=256)),
                ('content', models.CharField(max_length=4096)),
                ('order', models.IntegerField(default=0)),
                ('deleted', models.BooleanField(default=False)),
                ('created_datetime', models.DateTimeField(default=django.utils.timezone.now)),
                ('created_by_id', models.IntegerField(default=-1)),
                ('category', models.OneToOneField(blank=True, null=True, on_delete=django.db.models.deletion.PROTECT, to='core.category')),
                ('product', models.OneToOneField(blank=True, null=True, on_delete=django.db.models.deletion.PROTECT, to='core.product')),
            ],
        ),
    ]
