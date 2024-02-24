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





let score = 0;
let answeredQuestions = 0;
const totalQuestions = 4; // Nombre total de questions

function checkAnswer(answer, questionNumber) {
    const buttons = document.querySelectorAll(`.question${questionNumber} button`);
    buttons.forEach(button => {
        button.removeEventListener('click', () => {});
        button.disabled = true;
    });
    
    // Vérifier la réponse et mettre à jour le score
    if (questionNumber === 1 && (answer === 'Hawkins')) {
        score++;
    } else if (questionNumber === 2 && answer === 'Joyce') {
        score++;
    }else if (questionNumber === 3 && answer === 'Will') {
        score++;
    }else if (questionNumber === 4 && answer === 'Eleven') {
        score++;
    }
    
    
    answeredQuestions++;
    
    if (answeredQuestions === totalQuestions) {
        showResult();
        setTimeout(showFeedbackTextarea, 0); // Utiliser setTimeout avec un délai de 100 millisecondes pour retarder l'affichage de la zone de texte
    }
}

function showResult() {
    const resultContainer = document.getElementById('result');
    resultContainer.textContent = `Votre score est de ${score} sur ${totalQuestions}.`;
}

function showFeedbackTextarea() {
    const feedbackTextarea = document.getElementById('userFeedback');
    feedbackTextarea.style.display = 'block';
}


function submitFeedback() {
    const loadingOverlay = document.getElementById('loadingOverlay');
    const userFeedback = document.getElementById('userFeedback').value;
    
    // Afficher l'animation de chargement
    loadingOverlay.style.display = 'flex';
    
    // Simuler un délai de traitement (vous pouvez remplacer cela par votre logique de traitement réelle)
    setTimeout(() => {
        // Cacher l'animation de chargement après le traitement
        loadingOverlay.style.display = 'none';
        
        // Faire quelque chose avec l'avis de l'utilisateur, comme l'afficher ou l'envoyer à un serveur
        console.log("Avis de l'utilisateur :", userFeedback);
        
        // Afficher un message de confirmation
        alert("Merci pour votre avis ! L'avis a bien été pris en compte.");
        
        // Effacer le contenu de la zone de texte
        const feedbackTextarea = document.getElementById('userFeedback');
        feedbackTextarea.value = '';
    }, 2750); // Exemple de délai de 3 secondes (simule un traitement asynchrone)
}