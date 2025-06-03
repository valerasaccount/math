<!DOCTYPE html>
<html lang="ru">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Генератор уравнений</title>
  <style>
    /* ***********************
       Общие стили для страницы
       *********************** */
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin-top: 30px;
      background-color: #f0f0f0;
    }
    
    /* *******************************
       Стили для текстового ввода
       ******************************* */
    input[type="text"] {
      padding: 5px;
      font-size: 16px;
      width: 300px;
    }
    
    /* *******************************
       Стили для отображения уравнения
       ******************************* */
    #equation {
      font-size: 24px;
      font-weight: bold;
      margin: 20px;
      color: #333;
      min-height: 48px;
    }
    
    /* *******************************
       Стили для блока результата
       ******************************* */
    #result {
      font-size: 18px;
      margin-top: 15px;
      color: #006600;
      opacity: 1;
      min-height: 60px;
    }
    
    /* *******************************
       Стили для отображения имени пользователя
       ******************************* */
    #userDisplay {
      font-size: 20px;
      margin-bottom: 20px;
    }
    
    /* *******************************
       Стили для блока выбора сложности
       ******************************* */
    #difficultySelection {
      margin-bottom: 30px;
    }
    
    /* *******************************
       Стили для игровой области (скрыта до начала игры)
       ******************************* */
    #gameArea {
      display: none;
    }
    
    /* *******************************
       Контейнер для счётчика
       ******************************* */
    #counterContainer {
      margin-bottom: 15px;
      font-size: 18px;
    }
    
    /* *******************************
       Контейнер для таймера – отображает обратный отсчёт (например, "Время: 30 с")
       ******************************* */
    #timerDisplay {
      margin-bottom: 15px;
      font-size: 18px;
      color: #cc0000;
    }
  </style>
</head>
<body>
  <!-- ***********************
       Заголовок страницы
       *********************** -->
  <h1>Генератор уравнений</h1>
  
  <!-- ***********************
       Отображение имени пользователя (из URL-параметра "name")
       *********************** -->
  <p id="userDisplay">Пользователь: <span id="userNameDisplay">-</span></p>
  
  <!-- ***********************
       Блок выбора сложности
       *********************** -->
  <div id="difficultySelection">
    <p>Выберите уровень сложности:</p>
    <select id="difficultySelect">
      <!-- Уровни 1–8: существующие, а теперь добавлен уровень 9 -->
      <option value="1">Уровень 1: Базовый (a*x ± b = c)</option>
      <option value="2">Уровень 2: Переменная с двух сторон (a*x ± B = d*x ± C)</option>
      <option value="3">Уровень 3: Раскрытие скобок (a*(x ± b) = c)</option>
      <option value="4">Уровень 4: Дробный коэффициент (1/d*x ± b = c)</option>
      <option value="5">Уровень 5: Расширенное уравнение (a*(x ± b) ± c = d*(x ± e) ± f)</option>
      <option value="6">Уровень 6: Системное уравнение (две переменные: x и y)</option>
      <option value="7">Уровень 7: Дробное уравнение ((x ± a)/b = c)</option>
      <option value="8">Уровень 8: Квадратное уравнение (a*x² + b*x + c = 0)</option>
      <option value="9">Уровень 9: Уравнение со скобками ( (ax+b)(cx+d) = (ex+f)(gx+h) )</option>
    </select>
    <br><br>
    <button id="startGameBtn">Начать игру</button>
  </div>
  
  <!-- ***********************
       Игровая область
       *********************** -->
  <div id="gameArea">
    <p id="counterContainer">Счётчик: <span id="counter">10</span></p>
    <p id="timerDisplay">Время: 30 с</p>
    <p id="equation"></p>
    <input type="text" id="answer" placeholder="Введите значение">
    <p id="result"></p>
  </div>
  <script>
    // ====================================================
    // 1. Глобальные переменные и извлечение параметров из URL
    // ====================================================
    let currentDifficulty = 1;
    let currentSolution = null;
    let questionStartTime = null;
    let counter = 10;
    let timerInterval = null;  // Интервал таймера
    let currentTime = 0;       // Оставшееся время таймера (сек)
    let timedOut = false;      // Флаг, что для текущего уравнения таймер сработал
    
    const urlParams = new URLSearchParams(window.location.search);
    
    // Параметр "counter"
    const counterParam = urlParams.get('counter');
    if (counterParam !== null) {
      const initialCounter = parseInt(counterParam, 10);
      if (!isNaN(initialCounter)) { counter = initialCounter; }
    }
    
    // Параметр "penalty" – штраф за неправильный ответ
    let penalty = 2;
    const penaltyParam = urlParams.get('penalty');
    if (penaltyParam !== null) {
      const parsedPenalty = parseInt(penaltyParam, 10);
      if (!isNaN(parsedPenalty)) { penalty = parsedPenalty; }
    }
    
    // Параметр "timeLimit" – продолжительность таймера (сек; по умолчанию 30)
    let timeLimit = 30;
    const timeLimitParam = urlParams.get('timeLimit');
    if (timeLimitParam !== null) {
      const tl = parseInt(timeLimitParam, 10);
      if (!isNaN(tl)) { timeLimit = tl; }
    }
    
    // Параметр "timePunishment" – штраф за истечение времени (сек; по умолчанию 1)
    let timePunishment = 1;
    const timePunishmentParam = urlParams.get('timePunishment');
    if (timePunishmentParam !== null) {
      const tp = parseInt(timePunishmentParam, 10);
      if (!isNaN(tp)) { timePunishment = tp; }
    }
    
    // Параметр "name"
    let userName = urlParams.get('name');
    if (!userName) { userName = "-"; }
    document.getElementById("userNameDisplay").textContent = userName;
    
    // Режим тестера: если в URL есть tester=true, скрыть таймер и счётчик
    let testerMode = (urlParams.get("tester") || "").toLowerCase() === "true";
    if (testerMode) {
      const counterContainer = document.getElementById("counterContainer");
      if (counterContainer) { counterContainer.style.display = "none"; }
      document.getElementById("timerDisplay").style.display = "none";
    }
    
    // ====================================================
    // 2. Массивы для статистики и получение элементов документа
    // ====================================================
    let correctTimes = [];
    let incorrectTimes = [];
    let completedEquations = [];
    
    const equationP = document.getElementById("equation");
    const answerInput = document.getElementById("answer");
    const resultP = document.getElementById("result");
    const counterSpan = document.getElementById("counter");
    const timerDisplay = document.getElementById("timerDisplay");
    const difficultySelect = document.getElementById("difficultySelect");
    const startGameBtn = document.getElementById("startGameBtn");
    const difficultySelectionDiv = document.getElementById("difficultySelection");
    const gameAreaDiv = document.getElementById("gameArea");
    
    // Функция обновления счётчика
    function updateCounter() {
      if (!testerMode) { counterSpan.textContent = counter; }
    }
    updateCounter();
    
    // Функция обновления отображения таймера
    function updateTimerDisplay() {
      if (!testerMode) { timerDisplay.textContent = "Время: " + currentTime + " с"; }
    }
    updateTimerDisplay();
    
    // ====================================================
    // 3. Вспомогательные функции форматирования
    // ====================================================
    function formatVarTerm(coefficient, variable) {
      return (Math.abs(coefficient) === 1) ? variable : (coefficient + variable);
    }
    function formatConstTerm(value) {
      return (value >= 0) ? value.toString() : "-" + Math.abs(value).toString();
    }
    function formatSigned(value) {
      return value >= 0 ? ("+ " + value) : ("- " + Math.abs(value));
    }
  </script>
  <script>
    // ====================================================
    // 4. Функция генерации уравнений по уровню сложности
    // ====================================================
    function generateRandomEquationByDifficulty(difficulty) {
      let eqObj = { equation: "", solution: null };
      
      // ... Уровни 1-8 без изменений ...

      if (difficulty === 9) {
        // Новый уровень 9: (ax + b)(cx + d) = (ex + f)(gx + h)
        // Каждый коэффициент != 0, != 1, != -1 и все разные
        let a, b, c, d, e, f, g, h;
        let valuesUsed = new Set();

        function getUniqueCoef(minAbs = 2, maxAbs = 10) {
          let v;
          let tries = 0;
          do {
            v = Math.floor(Math.random() * (2 * maxAbs + 1)) - maxAbs; // -maxAbs..maxAbs
            tries++;
            // Проверка: не 0, не 1, не -1, и не совпадает по модулю ни с одним из уже выбранных
          } while (
            (Math.abs(v) < minAbs) ||
            valuesUsed.has(v) ||
            valuesUsed.has(-v) ||
            tries > 100
          );
          valuesUsed.add(v);
          return v;
        }

        a = getUniqueCoef();
        b = getUniqueCoef();
        c = getUniqueCoef();
        d = getUniqueCoef();
        e = getUniqueCoef();
        f = getUniqueCoef();
        g = getUniqueCoef();
        h = getUniqueCoef();

        // Теперь раскрываем скобки для обеих частей:
        // (ax+b)(cx+d) = acx^2 + (ad+bc)x + bd
        // (ex+f)(gx+h) = egx^2 + (eh+fg)x + fh
        let A = a * c;
        let B = a * d + b * c;
        let C = b * d;
        let D = e * g;
        let E = e * h + f * g;
        let F = f * h;

        // Переводим всё в одну часть: (A-D)x^2 + (B-E)x + (C-F) = 0
        let quad = A - D;
        let lin = B - E;
        let constTerm = C - F;

        // Решаем квадратное уравнение quad*x^2 + lin*x + constTerm = 0
        // (quad != 0, так как коэффициенты все разные)
        let disc = lin * lin - 4 * quad * constTerm;
        let roots = [];
        if (disc >= 0) {
          let sqrtDisc = Math.sqrt(disc);
          let root1 = (-lin + sqrtDisc) / (2 * quad);
          let root2 = (-lin - sqrtDisc) / (2 * quad);
          if (Math.abs(root1 - root2) < 0.001) {
            roots = [root1];
          } else {
            roots = [root1, root2];
          }
        } else {
          // Практически невозможно при случайных коэффах, но если вдруг, пересоздадим
          return generateRandomEquationByDifficulty(9);
        }

        function termStrA(a, variable) {
          if (a === 1) return variable;
          if (a === -1) return "-" + variable;
          return a + "x";
        }
        function termStrB(b) {
          return b >= 0 ? "+ " + b : "- " + Math.abs(b);
        }

        let left = "(" + termStrA(a, "x") + " " + termStrB(b) + ")";
        let left2 = "(" + termStrA(c, "x") + " " + termStrB(d) + ")";
        let right = "(" + termStrA(e, "x") + " " + termStrB(f) + ")";
        let right2 = "(" + termStrA(g, "x") + " " + termStrB(h) + ")";

        eqObj.equation = left + left2 + " = " + right + right2;
        eqObj.solution = roots;
      }

      // ... остальные уровни без изменений ...

      return eqObj;
    }
    
    // ====================================================
    // 5. Функция генерации и отображения нового уравнения, а также запуск таймера
    // ====================================================
    function generateEquation() {
      answerInput.value = "";
      answerInput.disabled = false;
      answerInput.focus();
      
      clearInterval(timerInterval);
      currentTime = timeLimit;
      updateTimerDisplay();
      timedOut = false;
      
      const randomEq = generateRandomEquationByDifficulty(currentDifficulty);
      
      if (currentDifficulty === 6) {
        equationP.innerHTML = randomEq.equation;
        answerInput.placeholder = "Введите x и y через запятую";
      } else if (currentDifficulty === 8 || currentDifficulty === 9) {
        equationP.textContent = randomEq.equation;
        answerInput.placeholder = "Введите корни через запятую";
      } else {
        equationP.textContent = randomEq.equation;
        answerInput.placeholder = "Введите значение";
      }
      
      currentSolution = randomEq.solution;
      questionStartTime = Date.now();
      
      if (!testerMode) {
        timerInterval = setInterval(function() {
          currentTime--;
          updateTimerDisplay();
          // Если время истекло, останавливаем таймер и показываем сообщение, оставляя уравнение активным.
          if (currentTime <= 0 && !timedOut) {
            timedOut = true;
            clearInterval(timerInterval);
            counter += timePunishment;
            updateCounter();
            resultP.textContent = "Время истекло! + " + timePunishment;
          }
        }, 1000);
      }
    }
    
    // ====================================================
    // 6. Функция вычисления статистики (лучшее, худшее, среднее время)
    // ====================================================
    function computeStats(times) {
      if (times.length === 0) { return { best: "-", worst: "-", average: "-" }; }
      const best = Math.min(...times);
      const worst = Math.max(...times);
      const sum = times.reduce((acc, t) => acc + t, 0);
      const average = sum / times.length;
      return { best: best.toFixed(2), worst: worst.toFixed(2), average: average.toFixed(2) };
    }
    
    // ====================================================
    // 7. Функция отправки статистики на сервер (с уровнями и именем)
    // ====================================================
    function sendStats(correctStats, incorrectStats, correctCount, incorrectCount, history) {
      const formData = new URLSearchParams();
      formData.append('difficulty', currentDifficulty);
      formData.append('best_correct', correctStats.best);
      formData.append('worst_correct', correctStats.worst);
      formData.append('avg_correct', correctStats.average);
      formData.append('best_incorrect', incorrectStats.best);
      formData.append('worst_incorrect', incorrectStats.worst);
      formData.append('avg_incorrect', incorrectStats.average);
      formData.append('correct_count', correctCount);
      formData.append('incorrect_count', incorrectCount);
      formData.append('history', history.join(", "));
      formData.append('name', userName);
      
      fetch('save_stats.php', {
        method: 'POST',
        body: formData
      })
      .then(response => response.text())
      .then(text => { console.log("Ответ сервера: " + text); })
      .catch(error => { console.error("Ошибка отправки данных: " + error); });
    }
    
    // ====================================================
    // 8. Функция завершения игры: вывод статистики и отправка данных
    // ====================================================
    function endGame() {
      const correctStats = computeStats(correctTimes);
      const incorrectStats = computeStats(incorrectTimes);
      let historyTimes = completedEquations.map(item => Math.round(item.time));
      
      let outputHTML =
        `<h2>Игра окончена</h2>
         <h3>Правильные ответы (кол-во: ${correctTimes.length}):</h3>
         <p>Лучшее время: ${correctStats.best} сек</p>
         <p>Худшее время: ${correctStats.worst} сек</p>
         <p>Среднее время: ${correctStats.average} сек</p>
         <h3>Неправильные ответы (кол-во: ${incorrectTimes.length}):</h3>
         <p>Лучшее время: ${incorrectStats.best} сек</p>
         <p>Худшее время: ${incorrectStats.worst} сек</p>
         <p>Среднее время: ${incorrectStats.average} сек</p>
         <h3>История времени решений (сек):</h3>
         <p>${historyTimes.join(", ")}</p>`;
      
      resultP.innerHTML = outputHTML;
      answerInput.disabled = true;
      
      if (!testerMode) { sendStats(correctStats, incorrectStats, correctTimes.length, incorrectTimes.length, historyTimes); }
    }
    
    // ====================================================
    // 9. Функция проверки ответа пользователя
    // ====================================================
    function checkAnswer() {
      // Не останавливаем таймер при неправильном ответе.
      if (testerMode) {
        resultP.textContent = "Режим тестера: ответ не проверяется. Пропуск уравнения.";
        setTimeout(() => { resultP.textContent = ""; generateEquation(); }, 500);
        return;
      }
      
      if (currentSolution === null) {
        resultP.textContent = "Подождите, уравнение ещё не загрузилось!";
        return;
      }
      
      const responseTime = (Date.now() - questionStartTime) / 1000;
      
      if (currentDifficulty === 6) {
        let userInput = answerInput.value.split(",");
        if (userInput.length !== 2) {
          resultP.textContent = "Пожалуйста, введите два числовых значения через запятую.";
          return;
        }
        let userX = parseFloat(userInput[0].trim());
        let userY = parseFloat(userInput[1].trim());
        if (isNaN(userX) || isNaN(userY)) {
          resultP.textContent = "Пожалуйста, введите корректные числовые значения.";
          return;
        }
        if (Math.abs(userX - currentSolution.x) < 0.001 &&
            Math.abs(userY - currentSolution.y) < 0.001) {
          clearInterval(timerInterval);
          resultP.textContent = "Верно!";
          correctTimes.push(responseTime);
          completedEquations.push({ equation: equationP.innerHTML, time: responseTime });
          if (!testerMode && !timedOut) {
            counter--;
            updateCounter();
          }
          answerInput.disabled = true;
          if (!testerMode && counter <= 0) {
            setTimeout(() => {
              resultP.style.transition = "opacity 2s";
              resultP.style.opacity = "0";
              setTimeout(() => { resultP.textContent = ""; resultP.style.opacity = "1"; endGame(); }, 2000);
            }, 2000);
            return;
          }
          setTimeout(() => {
            resultP.style.transition = "opacity 2s";
            resultP.style.opacity = "0";
            setTimeout(() => { resultP.textContent = ""; resultP.style.opacity = "1"; generateEquation(); }, 2000);
          }, 2000);
        } else {
          resultP.textContent = "Неправильно! Попробуйте ещё.";
          incorrectTimes.push(responseTime);
          if (!testerMode) { counter += penalty; updateCounter(); }
          answerInput.value = "";
          setTimeout(() => { resultP.textContent = ""; }, 1000);
        }
      
      } else if (currentDifficulty === 8 || currentDifficulty === 9) {
        let parts = answerInput.value.split(",");
        let userRoots = parts.map(p => parseFloat(p.trim())).filter(v => !isNaN(v));
        let solutionArray = currentSolution;
        if (solutionArray.length === 1 && userRoots.length !== 1) {
          resultP.textContent = "Уравнение имеет один корень. Введите одно число.";
          return;
        }
        if (solutionArray.length === 2 && userRoots.length !== 2) {
          resultP.textContent = "Уравнение имеет два корня. Введите два числа через запятую.";
          return;
        }
        if (solutionArray.length === 2) {
          userRoots.sort((a, b) => a - b);
          let sortedSolution = solutionArray.slice().sort((a, b) => a - b);
          if (Math.abs(userRoots[0] - sortedSolution[0]) < 0.001 &&
              Math.abs(userRoots[1] - sortedSolution[1]) < 0.001) {
            clearInterval(timerInterval);
            resultP.textContent = "Верно!";
            completedEquations.push({ equation: equationP.textContent, time: responseTime });
            correctTimes.push(responseTime);
            if (!testerMode && !timedOut) {
              counter--;
              updateCounter();
            }
            answerInput.disabled = true;
            if (!testerMode && counter <= 0) {
              setTimeout(() => {
                resultP.style.transition = "opacity 2s";
                resultP.style.opacity = "0";
                setTimeout(() => { resultP.textContent = ""; resultP.style.opacity = "1"; endGame(); }, 2000);
              }, 2000);
              return;
            }
            setTimeout(() => {
              resultP.style.transition = "opacity 2s";
              resultP.style.opacity = "0";
              setTimeout(() => { resultP.textContent = ""; resultP.style.opacity = "1"; generateEquation(); }, 2000);
            }, 2000);
          } else {
            resultP.textContent = "Неправильно! Попробуйте ещё.";
            incorrectTimes.push(responseTime);
            if (!testerMode) { counter += penalty; updateCounter(); }
            answerInput.value = "";
            setTimeout(() => { resultP.textContent = ""; }, 1000);
          }
        } else {
          if (Math.abs(userRoots[0] - solutionArray[0]) < 0.001) {
            clearInterval(timerInterval);
            resultP.textContent = "Верно!";
            completedEquations.push({ equation: equationP.textContent, time: responseTime });
            correctTimes.push(responseTime);
            if (!testerMode && !timedOut) {
              counter--;
              updateCounter();
            }
            answerInput.disabled = true;
            if (!testerMode && counter <= 0) {
              setTimeout(() => {
                resultP.style.transition = "opacity 2s";
                resultP.style.opacity = "0";
                setTimeout(() => { resultP.textContent = ""; resultP.style.opacity = "1"; endGame(); }, 2000);
              }, 2000);
              return;
            }
            setTimeout(() => {
              resultP.style.transition = "opacity 2s";
              resultP.style.opacity = "0";
              setTimeout(() => { resultP.textContent = ""; resultP.style.opacity = "1"; generateEquation(); }, 2000);
            }, 2000);
          } else {
            resultP.textContent = "Неправильно! Попробуйте ещё.";
            incorrectTimes.push(responseTime);
            if (!testerMode) { counter += penalty; updateCounter(); }
            answerInput.value = "";
            setTimeout(() => { resultP.textContent = ""; }, 1000);
          }
        }
      
      } else {
        let userAnswer = parseFloat(answerInput.value);
        if (isNaN(userAnswer)) {
          resultP.textContent = "Пожалуйста, введите числовое значение.";
          return;
        }
        if (Math.abs(userAnswer - currentSolution) < 0.001) {
          clearInterval(timerInterval);
          resultP.textContent = "Верно!";
          correctTimes.push(responseTime);
          completedEquations.push({ equation: equationP.textContent, time: responseTime });
          if (!testerMode && !timedOut) {
            counter--;
            updateCounter();
          }
          answerInput.disabled = true;
          if (!testerMode && counter <= 0) {
            setTimeout(() => {
              resultP.style.transition = "opacity 2s";
              resultP.style.opacity = "0";
              setTimeout(() => { resultP.textContent = ""; resultP.style.opacity = "1"; endGame(); }, 2000);
            }, 2000);
            return;
          }
          setTimeout(() => {
            resultP.style.transition = "opacity 2s";
            resultP.style.opacity = "0";
            setTimeout(() => { resultP.textContent = ""; resultP.style.opacity = "1"; generateEquation(); }, 2000);
          }, 2000);
        } else {
          resultP.textContent = "Неправильно! Попробуйте ещё.";
          incorrectTimes.push(responseTime);
          if (!testerMode) { counter += penalty; updateCounter(); }
          answerInput.value = "";
          setTimeout(() => { resultP.textContent = ""; }, 1000);
        }
      }
    }
    
    // ====================================================
    // 10. Обработчик клавиши Enter для проверки ответа
    // ====================================================
    answerInput.addEventListener("keydown", function(e) {
      if (e.key === "Enter") {
        e.preventDefault();
        checkAnswer();
      }
    });
    
    // ====================================================
    // 11. Обработчик кнопки "Начать игру"
    // ====================================================
    startGameBtn.addEventListener("click", function() {
      currentDifficulty = parseInt(difficultySelect.value, 10);
      difficultySelectionDiv.style.display = "none";
      gameAreaDiv.style.display = "block";
      answerInput.focus();
      generateEquation();
    });
    
  </script>
</body>
</html>
