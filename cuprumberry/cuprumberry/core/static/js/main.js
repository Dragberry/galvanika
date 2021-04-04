/* on page init for mobiles */
$(document).on('pageinit', function() {
	onPageResize();
});
/* On document ready */
$(document).ready(function() {
	onPageResize();
	$(window).resize(onPageResize);
});

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