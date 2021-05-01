/* on page init for mobiles */
$(document).on('pageinit', function() {
	onPageResize();
	setupMainMenu();
});
/* On document ready */
$(document).ready(function() {
	onPageResize();
	$(window).resize(onPageResize);
	setupMainMenu();
});

function setupMainMenu() {
    var mainNavBarCollapse = document.getElementById('mainNavBarCollapse')
    mainNavBarCollapse.addEventListener('shown.bs.collapse', function () {
        $('.fixed-top').addClass('mobile-navbar-show');
    });
    mainNavBarCollapse.addEventListener('hidden.bs.collapse', function () {
        $('.fixed-top').removeClass('mobile-navbar-show');
    });
}

/* Update body margin on screen resize */
function onPageResize() {
	var headerHeight = $('header').height();
	var bodyMarginTop = $('body').css('margin-top');

	console.log(headerHeight)
	console.log(bodyMarginTop)
	if (bodyMarginTop != headerHeight) {
		$("body").css('margin-top', headerHeight);
	}

	var bodyMarginBottom = $('body').css('margin-bottom');
	var footerHeight = $('footer').height();
	if (bodyMarginBottom + 10 != footerHeight) {
		$("body").css('margin-bottom', footerHeight + 50);
	}
}

function loadMorePosts(lastPost) {
    var form = $('#loadMorePostsForm');
    var url = form.attr('action');
    console.log('loadMorePosts', url)
    $.ajax({
        type: "GET",
        url: url,
        success: function(data)  {
        console.log('loadMorePosts success')
            $('#loadMorePostsBlock').replaceWith(data);
        }
    });
}