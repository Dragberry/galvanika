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

function prepareQuickOrder(productId, productName, productImageUrl) {
     $('#quickOrderProductId').text(productId)
     $('#quickOrderProductName').text(productName)
     $('#quickOrderProductImg').attr('src', productImageUrl)
     $('#quickOrderProductImg').attr('alt', productName)
     $( "input[name='productId']" ).val(productId)
}

function submitQuickOrder() {
    var form = $('#quickOrderForm');
    var url = form.attr('action');
    $.ajax({
        type: "POST",
        url: url,
        data: form.serialize(),
        success: function(data)  {
            for (var fieldName in data.fields) {
                var field = $(`[name=${fieldName}]`)
                field.removeClass('is-valid');
                field.removeClass('is-invalid');
                field.val('')
                field.siblings('.invalid-feedback').text()
            }
            $('#quickOrderModal').modal('hide');
            $('#quickOrderSubmittedToast').toast('show');
        },
        error: function(event) {
            var data = event.responseJSON;
            for (var fieldName in data.fields) {
                console.log(field, data.fields[fieldName]);
                var field = $(`[name=${fieldName}]`)
                if (data.fields[fieldName]) {
                    field.addClass('is-invalid');
                    field.removeClass('is-valid');
                    field.siblings('.invalid-feedback').text(data.fields[fieldName]);
                } else {
                    field.addClass('is-valid');
                    field.removeClass('is-invalid');
                    field.siblings('.invalid-feedback').text();
                }
            }
        }
    });
}