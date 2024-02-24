document.addEventListener('DOMContentLoaded', function () {
    document.getElementById('menuIcon').addEventListener('click', function () {
        var sidebar = document.getElementById('sidebar');
        if (window.getComputedStyle(sidebar).left === '-310px') {
            sidebar.style.left = '0';
        } else {
            sidebar.style.left = '-310px';
        }
    });
    
    var submenuLinks = document.querySelectorAll('.has-submenu');
    submenuLinks.forEach(function (link) {
        link.addEventListener('click', function () {
            link.classList.toggle('active');
            var submenu = link.nextElementSibling;
            if (window.getComputedStyle(submenu).display === 'block') {
                submenu.style.display = 'none';
            } else {
                submenu.style.display = 'block';
            }
        });
    });
});


document.addEventListener('DOMContentLoaded', function () {
    sidebar.style.left = '0';
});

document.addEventListener('DOMContentLoaded', function () {
    var submitButton = document.querySelector('button[type="submit"]');
    var form = document.querySelector('form');
    
    submitButton.addEventListener('click', function (event) {
        event.preventDefault();
        
        // Récupérez les valeurs des champs du formulaire
        var firstName = document.getElementById('name').value;
        var lastName = document.getElementById('nom').value; // Utilisez 'nom' au lieu de 'name'
        var email = document.getElementById('email').value; // Utilisez 'email' au lieu de 'mail'
        var message = document.getElementById('msg').value;
        
        // Construisez l'URL de redirection avec les paramètres du formulaire
        var redirectUrl = 'page-de-redirection.html?' +
        'firstName=' + encodeURIComponent(firstName) +
        '&lastName=' + encodeURIComponent(lastName) +
        '&email=' + encodeURIComponent(email) +
        '&message=' + encodeURIComponent(message);
        
        // Redirigez l'utilisateur vers la nouvelle page
        window.location.href = redirectUrl;
    });
    
    var menuIcon = document.getElementById('menuIcon');
    var sidebar = document.getElementById('sidebar');
    
    menuIcon.addEventListener('click', function () {
        sidebar.classList.toggle('active');
    });
});

function validateEmail() {
    var emailField = document.getElementById("email");
    var emailError = document.getElementById("emailError");
    
    var isValid = /\S+@\S+\.\S+/.test(emailField.value);
    
    if (!isValid) {
        emailError.textContent = "Veuillez entrer une adresse e-mail valide.";
        emailError.style.color = "red";
    } else {
        emailError.textContent = "";
    }
    
    return isValid;
}
function submitForm() {
    // Ajoutez votre logique d'animation ici
    showLoadingAnimation();
    
    // Après une courte pause (simulant le traitement), effectuez la redirection
    setTimeout(async function () {
        await delay(2000); // ajustez la durée selon vos besoins (2000 millisecondes = 2 secondes)
        window.location.href = "page-de-redirection.html";
    }, 0);
    
    return false; // Empêche la soumission du formulaire
}

function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function showLoadingAnimation() {
    var loadingMessage = document.getElementById("loadingMessage");
    loadingMessage.innerHTML = '<div class="spinner"></div> Envoi en cours...';
}