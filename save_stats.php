<?php
// Параметры подключения к базе данных
$servername = "localhost";
$username   = "root";
$password   = "tarhush";
$dbname     = "cw";

// Создание подключения
$conn = new mysqli($servername, $username, $password, $dbname);

// Проверка соединения
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Проверяем, что все необходимые поля переданы через POST
if (
    isset(
      $_POST['difficulty'], 
      $_POST['best_correct'], 
      $_POST['worst_correct'], 
      $_POST['avg_correct'], 
      $_POST['best_incorrect'], 
      $_POST['worst_incorrect'], 
      $_POST['avg_incorrect'],
      $_POST['correct_count'], 
      $_POST['incorrect_count'], 
      $_POST['history'],
      $_POST['name']
    )
) {
    // Получение данных и преобразование в нужные типы
    $difficulty    = intval($_POST['difficulty']);
    $best_correct  = floatval($_POST['best_correct']);
    $worst_correct = floatval($_POST['worst_correct']);
    $avg_correct   = floatval($_POST['avg_correct']);
    
    $best_incorrect  = floatval($_POST['best_incorrect']);
    $worst_incorrect = floatval($_POST['worst_incorrect']);
    $avg_incorrect   = floatval($_POST['avg_incorrect']);
    
    $correct_count   = intval($_POST['correct_count']);
    $incorrect_count = intval($_POST['incorrect_count']);
    
    $history = $_POST['history'];
    $playerName = $_POST['name'];
    
    // Подготовка SQL-запроса с добавлением имени
    $stmt = $conn->prepare("INSERT INTO game_stats 
        (difficulty, best_correct, worst_correct, avg_correct, best_incorrect, worst_incorrect, avg_incorrect, correct_count, incorrect_count, history, name, created_at) 
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, NOW())");
    
    if ($stmt === false) {
        die("Ошибка подготовки запроса: " . $conn->error);
    }
    
    // Привязка параметров: 
    // "i" - difficulty,
    // 6 параметров с плавающей точкой ("d"),
    // 2 целых числа ("i"), 2 строки ("s")
    $stmt->bind_param("iddddddiiss", 
        $difficulty, 
        $best_correct, 
        $worst_correct, 
        $avg_correct, 
        $best_incorrect, 
        $worst_incorrect, 
        $avg_incorrect, 
        $correct_count, 
        $incorrect_count, 
        $history,
        $playerName
    );
    
    if ($stmt->execute()) {
        echo "Статистика игры успешно сохранена!";
    } else {
        echo "Ошибка: " . $stmt->error;
    }
    
    $stmt->close();
} else {
    echo "Не переданы необходимые данные.";
}

$conn->close();
?>
