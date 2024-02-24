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