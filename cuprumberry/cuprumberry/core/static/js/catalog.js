function onDisplayChanged() {
	$('input[name="display"]').parent().removeClass('btn-secondary');
	$('input[name="display"]:checked').parent().addClass('btn-secondary');
}

var delayTimer;

function doFilter() {
    console.log('doFilter')
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
	}, 1000);
}