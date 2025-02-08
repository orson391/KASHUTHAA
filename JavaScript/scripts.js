const socket = new WebSocket('ws://yourserver.com'); // Replace with your server URL

        socket.onmessage = function(event) {
            const data = JSON.parse(event.data);
            if (data.type === 'updateHand') {
                updateHand(data.cards);
            } else if (data.type === 'updateCenter') {
                updateCenterCards(data.cards);
            } else if (data.type === 'updatePlayers') {
                updatePlayers(data.players);
            }
        };

        function updateHand(cards) {
            const handDiv = document.getElementById('hand');
            handDiv.innerHTML = '';
            cards.forEach(card => {
                const cardElement = document.createElement('div');
                cardElement.className = 'card';
                cardElement.textContent = card;
                cardElement.onclick = () => playCard(card);
                handDiv.appendChild(cardElement);
            });
        }

        function updateCenterCards(cards) {
            const centerDiv = document.getElementById('center-cards');
            centerDiv.innerHTML = '';
            cards.forEach(card => {
                const cardElement = document.createElement('div');
                cardElement.className = 'card';
                cardElement.textContent = card;
                centerDiv.appendChild(cardElement);
            });
        }

        function updatePlayers(players) {
            const playersDiv = document.getElementById('players');
            playersDiv.innerHTML = '';
            players.forEach(player => {
                const playerDiv = document.createElement('div');
                playerDiv.className = 'player';
                playerDiv.textContent = player.name + ': ' + player.score;
                playersDiv.appendChild(playerDiv);
            });
        }

        function playCard(card) {
            socket.send(JSON.stringify({ type: 'playCard', card }));
        }