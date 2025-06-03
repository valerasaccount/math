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
      <option value="9">Уровень 9: Уравнение со скобками ( (ax+b)(cx+d)=e )</option>
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
  </script>
  <script>
    // ====================================================
    // 4. Функция генерации уравнений по уровню сложности
    // ====================================================
    function generateRandomEquationByDifficulty(difficulty) {
      let eqObj = { equation: "", solution: null };
      
      if (difficulty === 1) {
        let a, xSolution, b, c;
        do {
          a = Math.floor(Math.random() * 9) + 2;
          if (Math.random() < 0.5) { a = -a; }
          xSolution = Math.floor(Math.random() * 11);
          b = (Math.random() < 0.5) ? Math.floor(Math.random() * 20) + 1
                                    : - (Math.floor(Math.random() * 20) + 1);
          c = a * xSolution + b;
        } while (b === 0 || Math.abs(b) === Math.abs(a) ||
                 Math.abs(a) === 1 || Math.abs(c) === Math.abs(a) ||
                 Math.abs(c) === Math.abs(b) || Math.abs(c) === xSolution);
        let termVar = formatVarTerm(a, "x");
        let left = (Math.random() < 0.5)
          ? (formatConstTerm(b) + " + " + termVar)
          : (termVar + (b >= 0 ? " + " + b : " - " + Math.abs(b)));
        eqObj.equation = left + " = " + c;
        eqObj.solution = xSolution;
      
      } else if (difficulty === 2) {
        let a, d, xSolution, B, C;
        do {
          a = Math.floor(Math.random() * 9) + 2;
          if (Math.random() < 0.5) { a = -a; }
          do { d = Math.floor(Math.random() * 9) + 2; if (Math.random() < 0.5) { d = -d; } }
          while (Math.abs(d) === Math.abs(a));
          xSolution = Math.floor(Math.random() * 11);
          B = (Math.random() < 0.5) ? Math.floor(Math.random() * 21) : -Math.floor(Math.random() * 21);
          C = (a - d) * xSolution + B;
        } while (B === 0 || Math.abs(B) === Math.abs(a) ||
                 Math.abs(a) === 1 || Math.abs(d) === 1 ||
                 Math.abs(a) === xSolution || Math.abs(d) === xSolution ||
                 Math.abs(B) === xSolution || Math.abs(C) === xSolution);
        let leftTermVar = formatVarTerm(a, "x");
        let rightTermVar = formatVarTerm(d, "x");
        let leftSide = (Math.random() < 0.5)
          ? (formatConstTerm(B) + " + " + leftTermVar)
          : (leftTermVar + (B === 0 ? "" : (B >= 0 ? " + " + B : " - " + Math.abs(B))));
        let rightSide = (Math.random() < 0.5)
          ? (formatConstTerm(C) + " + " + rightTermVar)
          : (rightTermVar + (C === 0 ? "" : (C >= 0 ? " + " + C : " - " + Math.abs(C))));
        eqObj.equation = leftSide + " = " + rightSide;
        eqObj.solution = xSolution;
      
      } else if (difficulty === 3) {
        let a, xSolution, b, c;
        do {
          a = Math.floor(Math.random() * 9) + 2;
          if (Math.random() < 0.5) { a = -a; }
          xSolution = Math.floor(Math.random() * 11);
          b = (Math.random() < 0.5)
              ? Math.floor(Math.random() * 21)
              : -Math.floor(Math.random() * 21);
          c = a * (xSolution + b);
        } while (b === 0 || Math.abs(b) === Math.abs(a) ||
                 Math.abs(a) === 1 || Math.abs(a) === xSolution || Math.abs(b) === xSolution);
        let normalInner = (b === 0) ? "x" : (b > 0 ? "x + " + b : "x - " + Math.abs(b));
        let swappedInner = (b === 0) ? "x" : (formatConstTerm(b) + " + x");
        let inner = (Math.random() < 0.5) ? swappedInner : normalInner;
        let aStr = (Math.abs(a) === 1) ? "" : a.toString();
        eqObj.equation = aStr + "(" + inner + ") = " + c;
        eqObj.solution = xSolution;
      
      } else if (difficulty === 4) {
        let d, k, xSolution, b, c;
        d = Math.floor(Math.random() * 8) + 2;
        if (Math.random() < 0.5) { d = -d; }
        k = Math.floor(Math.random() * 11) - 5;
        xSolution = d * k;
        do {
          b = (Math.random() < 0.5)
              ? Math.floor(Math.random() * 21)
              : -Math.floor(Math.random() * 21);
        } while (b === 0 || Math.abs(b) === Math.abs(xSolution));
        c = k + b;
        let baseStr = "1/" + d + "x";
        let normalLeft = baseStr + (b === 0 ? "" : (b >= 0 ? " + " + b : " - " + Math.abs(b)));
        let swappedLeft = (b === 0) ? baseStr : (formatConstTerm(b) + " + " + baseStr);
        let left = (Math.random() < 0.5) ? swappedLeft : normalLeft;
        eqObj.equation = left + " = " + c;
        eqObj.solution = xSolution;
      
      } else if (difficulty === 5) {
        let a, d, xSolution, b, c_const, e, f_const;
        do {
          a = Math.floor(Math.random() * 9) + 2;
          if (Math.random() < 0.5) { a = -a; }
          do { d = Math.floor(Math.random() * 9) + 2; if (Math.random() < 0.5) { d = -d; } }
          while (Math.abs(d) === Math.abs(a));
          xSolution = Math.floor(Math.random() * 21) - 10;
          b = (Math.random() < 0.5) ? Math.floor(Math.random() * 21) : -Math.floor(Math.random() * 21);
          c_const = (Math.random() < 0.5) ? Math.floor(Math.random() * 21) : -Math.floor(Math.random() * 21);
          e = (Math.random() < 0.5) ? Math.floor(Math.random() * 21) : -Math.floor(Math.random() * 21);
          f_const = (Math.random() < 0.5) ? Math.floor(Math.random() * 21) : -Math.floor(Math.random() * 21);
        } while (
          b === 0 || c_const === 0 || e === 0 || f_const === 0 ||
          Math.abs(b) === Math.abs(a) ||
          Math.abs(a) === 1 || Math.abs(d) === 1 ||
          Math.abs(a) === Math.abs(xSolution) || Math.abs(d) === Math.abs(xSolution) ||
          Math.abs(b) === Math.abs(xSolution) || Math.abs(c_const) === Math.abs(xSolution) ||
          Math.abs(e) === Math.abs(xSolution) || Math.abs(f_const) === Math.abs(xSolution)
        );
        let normalInnerL = (b === 0) ? "x" : (b > 0 ? "x + " + b : "x - " + Math.abs(b));
        let swappedInnerL = (b === 0) ? "x" : (formatConstTerm(b) + " + x");
        let innerL = (Math.random() < 0.5) ? swappedInnerL : normalInnerL;
        let leftTerm = (Math.abs(a) === 1) ? innerL : a + "(" + innerL + ")";
        let normalLeft = leftTerm + (c_const === 0 ? "" : (c_const > 0 ? " + " + c_const : " - " + Math.abs(c_const)));
        let swappedLeft = (c_const === 0) ? leftTerm : (formatConstTerm(c_const) + " + " + leftTerm);
        let leftExpr = (Math.random() < 0.5) ? swappedLeft : normalLeft;
        
        let normalInnerR = (e === 0) ? "x" : (e > 0 ? "x + " + e : "x - " + Math.abs(e));
        let swappedInnerR = (e === 0) ? "x" : (formatConstTerm(e) + " + x");
        let innerR = (Math.random() < 0.5) ? swappedInnerR : normalInnerR;
        let rightTerm = (Math.abs(d) === 1) ? innerR : d + "(" + innerR + ")";
        let normalRight = rightTerm + (f_const === 0 ? "" : (f_const > 0 ? " + " + f_const : " - " + Math.abs(f_const)));
        let swappedRight = (f_const === 0) ? rightTerm : (formatConstTerm(f_const) + " + " + rightTerm);
        let rightExpr = (Math.random() < 0.5) ? swappedRight : normalRight;
        
        eqObj.equation = leftExpr + " = " + rightExpr;
        eqObj.solution = xSolution;
      
      } else if (difficulty === 6) {
        let xSolution = Math.floor(Math.random() * 21) - 10;
        let ySolution = Math.floor(Math.random() * 21) - 10;
        let a1, b1, a2, b2;
        do {
          a1 = Math.floor(Math.random() * 9) + 2;
          if (Math.random() < 0.5) { a1 = -a1; }
          b1 = Math.floor(Math.random() * 21) - 10;
          a2 = Math.floor(Math.random() * 9) + 2;
          if (Math.random() < 0.5) { a2 = -a2; }
          b2 = Math.floor(Math.random() * 21) - 10;
        } while (a1 * b2 - a2 * b1 === 0);
        let c1 = a1 * xSolution + b1 * ySolution;
        let c2 = a2 * xSolution + b2 * ySolution;
        function formatTerm(coefficient, variable) {
          if (coefficient === 1) return variable;
          if (coefficient === -1) return "-" + variable;
          return coefficient + variable;
        }
        let eq1 = formatTerm(a1, "x") + (b1 >= 0 ? " + " + b1 + "y" : " - " + Math.abs(b1) + "y") + " = " + c1;
        let eq2 = formatTerm(a2, "x") + (b2 >= 0 ? " + " + b2 + "y" : " - " + Math.abs(b2) + "y") + " = " + c2;
        eqObj.equation = eq1 + "<br>" + eq2;
        eqObj.solution = { x: xSolution, y: ySolution };
      
      } else if (difficulty === 7) {
        let b;
        do { b = Math.floor(Math.random() * 21) - 10; } while (b === 0);
        let a = Math.floor(Math.random() * 21) - 10;
        let c = Math.floor(Math.random() * 21) - 10;
        let xSolution = c * b - a;
        let inner;
        if (a === 0) { inner = "x"; }
        else if (a > 0) { inner = "x + " + a; }
        else { inner = "x - " + Math.abs(a); }
        eqObj.equation = "(" + inner + ")/" + b + " = " + c;
        eqObj.solution = xSolution;
      
      } else if (difficulty === 8) {
        let a = Math.floor(Math.random() * 9) + 1;
        let r1 = Math.floor(Math.random() * 21) - 10;
        let r2 = Math.floor(Math.random() * 21) - 10;
        let bCoeff = -a * (r1 + r2);
        let cCoeff = a * r1 * r2;
        function formatCoeff(coefficient, variable, isFirst) {
          if (coefficient === 0) return "";
          if (isFirst) {
            if (coefficient === 1) return variable;
            if (coefficient === -1) return "-" + variable;
            return coefficient + variable;
          } else {
            if (coefficient > 0) {
              if (coefficient === 1) return " + " + variable;
              return " + " + coefficient + variable;
            } else {
              if (coefficient === -1) return " - " + variable;
              return " - " + Math.abs(coefficient) + variable;
            }
          }
        }
        let eqStr = "";
        eqStr += (a === 1) ? "x²" : a + "x²";
        eqStr += formatCoeff(bCoeff, "x", false);
        if (cCoeff > 0) { eqStr += " + " + cCoeff; }
        else if (cCoeff < 0) { eqStr += " - " + Math.abs(cCoeff); }
        eqStr += " = 0";
        eqObj.equation = eqStr;
        eqObj.solution = (r1 === r2) ? [r1] : [r1, r2];
      
      } else if (difficulty === 9) {
        // Новый уровень 9: уравнение вида (ax+b)(cx+d)=e, которое раскрывается в квадратное уравнение.
        // Значения a, b, c, d, e не должны быть 0 или ±1, и их абсолютные значения должны быть все различны.
        let a, b, c, d, k, e, A, B, C, disc;
        while (true) {
          // Генерируем a, b, c, d с абсолютным значением не меньше 2
          a = Math.floor(Math.random() * 11) - 5;
          if (Math.abs(a) < 2) a = a < 0 ? -2 : 2;
          
          c = Math.floor(Math.random() * 11) - 5;
          if (Math.abs(c) < 2) c = c < 0 ? -3 : 3;
          
          b = Math.floor(Math.random() * 21) - 10;
          if (Math.abs(b) < 2) b = b < 0 ? -2 : 2;
          
          d = Math.floor(Math.random() * 21) - 10;
          if (Math.abs(d) < 2) d = d < 0 ? -3 : 3;
          
          // Проверяем, чтобы абсолютные значения a, b, c, d были уникальны
          let uniqueAbs = new Set([Math.abs(a), Math.abs(b), Math.abs(c), Math.abs(d)]);
          if (uniqueAbs.size !== 4) continue;
          
          // k выбирается из диапазона [1,5]
          k = Math.floor(Math.random() * 5) + 1;
          // Определяем e как b*d - k
          e = b * d - k;
          if (Math.abs(e) < 2) continue;
          
          // Проверяем, что абсолютное значение e отличается от остальных
          uniqueAbs.add(Math.abs(e));
          if (uniqueAbs.size !== 5) continue;
          
          // Вычисляем коэффициенты раскрытия: A = a*c, B = a*d + b*c, C = k.
          A = a * c;
          B = a * d + b * c;
          C = k;
          disc = B * B - 4 * A * C;
          if (disc < 0) continue;
          
          // Нашли подходящие значения
          break;
        }
        // Вычисляем корни квадратного уравнения (для информации)
        let sqrtDisc = Math.sqrt(disc);
        let root1 = (-B + sqrtDisc) / (2 * A);
        let root2 = (-B - sqrtDisc) / (2 * A);
        // Формируем строку уравнения вида (ax+b)(cx+d)=e
        eqObj.equation = "(" + a + "x " + (b >= 0 ? "+ " + b : "- " + Math.abs(b)) + ")" +
                         "(" + c + "x " + (d >= 0 ? "+ " + d : "- " + Math.abs(d)) + ") = " + e;
        // Сохраняем корни как решение (с проверкой на совпадение)
        if (Math.abs(root1 - root2) < 0.001) {
          eqObj.solution = [root1];
        } else {
          eqObj.solution = [root1, root2];
        }
      }
      
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
      } else if (currentDifficulty === 8) {
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
      
      } else if (currentDifficulty === 8) {
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
        } else {
          if (Math.abs(userRoots[0] - solutionArray[0]) < 0.001) {
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
