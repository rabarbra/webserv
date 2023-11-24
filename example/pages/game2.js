document.getElementById('startGame2').addEventListener('click', function () {
    const resultElement = document.getElementById('game2Result');
    const isSuccess = Math.random() > 0.5;
    resultElement.textContent = isSuccess ? 'Firewall breached! Access granted.' : 'Access denied. Firewall intact.';
    resultElement.style.color = isSuccess ? '#0F0' : '#F00';
});
