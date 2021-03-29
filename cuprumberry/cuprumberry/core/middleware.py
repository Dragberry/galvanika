from typing import Tuple

from .models import Category, Social


class MainMenuMiddleware:
    CATEGORY_METAL: int = 10000
    CATEGORY_DECORATIONS: int = 20000
    CATEGORY_BASE: int = 30000

    def __init__(self, get_response):
        self.get_response = get_response
        self.decorations: [Category] = Category.objects.filter(parent=MainMenuMiddleware.CATEGORY_DECORATIONS)
        self.category_base: Category = Category.objects.get(id=MainMenuMiddleware.CATEGORY_BASE)
        self.bases: [Category] = Category.objects.filter(parent=MainMenuMiddleware.CATEGORY_BASE)
        self.category_metal: Category = Category.objects.get(id=MainMenuMiddleware.CATEGORY_METAL)
        self.metals: [Category] = Category.objects.filter(parent=MainMenuMiddleware.CATEGORY_METAL)
        self.socials: [Social] = Social.objects.all()

    def __call__(self, request):
        request.decorations = self.decorations
        request.category_base = self.category_base
        request.bases = self.bases
        request.category_metal = self.category_metal
        request.metals = self.metals
        request.socials = self.socials
        response = self.get_response(request)
        return response
