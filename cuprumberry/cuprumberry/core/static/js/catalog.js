/* on page init for mobiles */
$(document).on('pageinit', function() {
	addEventListeners();
});
/* On document ready */
$(document).ready(function() {
    addEventListeners();
});

function addEventListeners() {
    var productCarouselIndicators = document.getElementById('productCarouselIndicators');
    if (productCarouselIndicators) {
        productCarouselIndicators.addEventListener('slide.bs.carousel', function (event) {
            var $thumbnails = $('.carousel-thumbnails');
            var $thumbnail_prev = $(`.img-carousel-thumbnail[data-bs-slide-to="${event.from}"]`);
            var $thumbnail_next = $(`.img-carousel-thumbnail[data-bs-slide-to="${event.to}"]`);
            var slidesWidth = $thumbnails.width();
            var scrollLeft = ($thumbnail_next.outerWidth() * (0.5 + event.to)) - (slidesWidth / 2);
            $thumbnails.animate({scrollLeft: scrollLeft}, 600, 'linear');
            $thumbnail_prev.removeClass('active');
            $thumbnail_next.addClass('active');
        });
    }
}

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

function showMoreLongDescription() {
    $('#longDescription').removeClass('d-inline-block text-truncate w-50 w-75');
    $('#showLongDescription').remove();
}

/* global bootstrap: false */
(function () {
  'use strict'
  var tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'))
  tooltipTriggerList.forEach(function (tooltipTriggerEl) {
    new bootstrap.Tooltip(tooltipTriggerEl)
  })
})()

function processCartChange(data) {
    var $cartProductCountBadge = $('.cartProductCountBadge')
    var cartProductCount = data['cartProductCount'];
    $cartProductCountBadge.text(cartProductCount)
    if (cartProductCount > 0) {
        $cartProductCountBadge.removeClass('d-none');
    } else {
        $cartProductCountBadge.addClass('d-none');
    }
}

function addToCart(productId) {
    $('#addToCartForm > :input[name="productId"]').val(productId)

    var $form = $('#addToCartForm');
    var url = $form.attr('action');

    $.ajax({
        type: "POST",
        url: url,
        data: $form.serialize(),
        success: function(data)  {
            processCartChange(data);
            $('#itemAddedToCartProductId').text(data['productId'])
            $('#itemAddedToCartProductName').text(data['productName'])
            $('#itemAddedToCartProductImg').attr('src', data['productImageUrl'])
            $('#itemAddedToCartProductImg').attr('alt', data['productName'])
            $('#itemAddedToCartToast').toast('show');
        },
         error: function($xhr, textStatus, errorThrown){
            var data = $xhr.responseJSON;
            $('#itemNotAddedToCartToast').toast('show');
            $('#itemNotAddedToCartError').text(data['error'])
         }
    });
}

function removeItemFromCart(productId) {
    var form = $('#cartItemsForm');
    var url = form.attr('action');
    $.ajax({
        type: "GET",
        url: url,
        data: `productId=${productId}`,
        success: function(data)  {
            processCartChange(data);
            $('#cartContent').fadeOut('normal', function() {
                $(this).replaceWith(data['cartProductList']);
            });
        }
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