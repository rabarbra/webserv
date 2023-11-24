document.getElementById('startGame1').addEventListener('click', function () {
    const resultElement = document.getElementById('game1Result');
    const isSuccess = Math.random() > 0.5;
    resultElement.textContent = isSuccess ? 'Password cracked! Access granted.' : 'Access denied. Try again.';
    resultElement.style.color = isSuccess ? '#0F0' : '#F00';
});
