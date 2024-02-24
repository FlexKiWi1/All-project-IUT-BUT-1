    document.addEventListener('DOMContentLoaded', function () {
        document.getElementById('menuIcon').addEventListener('click', function () {
            var sidebar = document.getElementById('sidebar');
            var body = document.getElementById('body');
            sidebar.style.left = (sidebar.style.left === '-310px') ? '0' : '-310px';
            if(sidebar.style.left === '-310px'){
                if(window.innerWidth > 1000){
                    body.style.marginLeft = '-6em';
                    body.style.width = '100%';
                }
            }
            else if (window.innerWidth > 1000) {
                body.style.marginLeft = '2em';
                body.style.width = '100%';
            }
        });
        
        var submenuLinks = document.querySelectorAll('.has-submenu');
        submenuLinks.forEach(function (link) {
            link.addEventListener('click', function () {
                link.classList.toggle('active');
                var submenu = link.nextElementSibling;
                submenu.style.display = (link.classList.contains('active')) ? 'block' : 'none';
            });
        });
    });
    
    document.addEventListener('DOMContentLoaded', function () {
        if (window.innerWidth <= 428) {
            const containerDivs = document.querySelectorAll('.container2 > div.transition');
            
            containerDivs.forEach(function (div) {
                div.addEventListener('click', function () {
                    div.classList.toggle('active');
                    var sidebar = document.getElementById('sidebar');
                    
                    document.getElementById('menuIcon').addEventListener('click', function () {
                        var sidebar = document.getElementById('sidebar');
                        sidebar.style.height = (sidebar.style.height === '') ? '0' : '-310px';
                    });

                    containerDivs.forEach(function (otherDiv) {
                        if (otherDiv !== div) {
                            otherDiv.classList.remove('active');
                        }
                    });
                    
                    if (div.classList.contains('active')) {
                        div.style.height = div.scrollHeight + 'px';
                    } else {
                        div.style.height = '';
                    }
                });
            });
        }
    });
    