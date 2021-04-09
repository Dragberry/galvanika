import babel.dates
from django.template.defaultfilters import register


@register.filter
def format_date(value):
    return babel.dates.format_date(value, locale='ru_RU')