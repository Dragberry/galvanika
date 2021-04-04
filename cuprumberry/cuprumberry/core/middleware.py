from .models import Category, Social


class MainMenuMiddleware:
    CATEGORY_METAL: str = "izdeliya-iz-metalla"
    CATEGORY_SOUVENIRS: str = "suveniry"
    CATEGORY_DECORATIONS: str = "bizhuteriya"
    CATEGORY_BASE: str = "osnova"

    def __init__(self, get_response):
        self.get_response = get_response
        self.category_decorations: Category = Category.objects.get(id=MainMenuMiddleware.CATEGORY_DECORATIONS)
        self.decorations: [Category] = Category.objects.filter(parent=MainMenuMiddleware.CATEGORY_DECORATIONS)
        self.category_base: Category = Category.objects.get(id=MainMenuMiddleware.CATEGORY_BASE)
        self.bases: [Category] = Category.objects.filter(parent=MainMenuMiddleware.CATEGORY_BASE).order_by('name')
        self.category_metal: Category = Category.objects.get(id=MainMenuMiddleware.CATEGORY_METAL)
        self.category_souvenirs: Category = Category.objects.get(id=MainMenuMiddleware.CATEGORY_SOUVENIRS)
        self.metals: [Category] = Category.objects.filter(parent=MainMenuMiddleware.CATEGORY_METAL)
        self.socials: [Social] = Social.objects.all()

    def __call__(self, request):
        request.category_decorations = self.category_decorations
        request.decorations = self.decorations
        request.category_base = self.category_base
        request.bases = self.bases
        request.category_metal = self.category_metal
        request.metals = self.metals
        request.socials = self.socials
        request.category_souvenirs = self.category_souvenirs
        response = self.get_response(request)
        return response
