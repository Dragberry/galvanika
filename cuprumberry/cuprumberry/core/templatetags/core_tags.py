import babel.dates
from django.template.defaultfilters import register


@register.filter
def format_date(value):
    return babel.dates.format_date(value, locale='ru_RU')


@register.filter
def hash(h, key):
    return h.get(key) if isinstance(h, dict) else ''


@register.filter
def has(h, key):
    return h and key in h
