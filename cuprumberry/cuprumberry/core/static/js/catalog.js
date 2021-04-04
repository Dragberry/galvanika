/* on page init for mobiles */
$(document).on('pageinit', function() {
	addEventListeners();
});
/* On document ready */
$(document).ready(function() {
	addEventListeners();
});

var delayTimer;

function doFilter(delay = 1000) {
    clearTimeout(delayTimer);
	delayTimer = setTimeout(function() {
		var form = $('#filtersForm');
	    var url = form.attr('action');
	    $.ajax({
	    	type: "GET",
	        url: url,
	        data: form.serialize(),
	        success: function(data)  {
	        	$('#productList').fadeOut('normal', function() {
	        		$(this).replaceWith(data);
	        	});
	        }
	   	});
	}, delay);
}

function addEventListeners() {
    $('[id^=collapsedMenu_]').each(function() {
        this.addEventListener('shown.bs.collapse', function (event) {
            var icon = $(event.target).siblings('span').children('span')
            icon.removeClass('fa-angle-double-right')
            icon.addClass('fa-angle-double-down')
        });
        this.addEventListener('hidden.bs.collapse', function (event) {
            var icon = $(event.target).siblings('span').children('span')
            icon.removeClass('fa-angle-double-down')
            icon.addClass('fa-angle-double-right')
        });
    });
}
