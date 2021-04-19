from django.core.exceptions import ObjectDoesNotExist
from django.db.models import Q

from .models import Category, Social


class MainMenuMiddleware:
    CATEGORY_METAL: str = "izdeliya-iz-metalla"
    CATEGORY_SOUVENIRS: str = "suveniry"
    CATEGORY_DECORATIONS: str = "bizhuteriya"
    CATEGORY_BASE: str = "osnova"

    def __init__(self, get_response):
        self.get_response = get_response
        try:
            self.category_decorations: Category = Category.objects.get(Q(id=MainMenuMiddleware.CATEGORY_DECORATIONS) & Q(deleted=False))
        except ObjectDoesNotExist:
            self.category_decorations = None
        try:
            self.information: Category = Category.objects.get(Q(id=MainMenuMiddleware.CATEGORY_BASE) & Q(deleted=False))
        except ObjectDoesNotExist:
            self.information = None
        try:
            self.category_metal: Category = Category.objects.get(Q(id=MainMenuMiddleware.CATEGORY_METAL) & Q(deleted=False))
        except ObjectDoesNotExist:
            self.category_metal = None
        try:
            self.category_souvenirs: Category = Category.objects.get(Q(id=MainMenuMiddleware.CATEGORY_SOUVENIRS) & Q(deleted=False))
        except ObjectDoesNotExist:
            self.category_souvenirs = None
        self.decorations: [Category] = Category.objects.filter(Q(parent=MainMenuMiddleware.CATEGORY_DECORATIONS) & Q(deleted=False))
        self.bases: [Category] = Category.objects.filter(Q(parent=MainMenuMiddleware.CATEGORY_BASE) & Q(deleted=False)).order_by('name')
        self.metals: [Category] = Category.objects.filter(Q(parent=MainMenuMiddleware.CATEGORY_METAL) & Q(deleted=False))
        self.socials: [Social] = Social.objects.all()

    def __call__(self, request):
        request.category_decorations = self.category_decorations
        request.decorations = self.decorations
        request.category_base = self.information
        request.bases = self.bases
        request.category_metal = self.category_metal
        request.metals = self.metals
        request.socials = self.socials
        request.category_souvenirs = self.category_souvenirs
        request.app_name = 'CuprumBerry.by'
        request.app_email = 'cuprumberry@cuprumberry.by'
        request.app_mobile = '+375(29)114-47-50'
        response = self.get_response(request)
        return response
