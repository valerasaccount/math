<?php
// index.php

// 1) Параметры подключения — замените на свои
$db_host = 'localhost';
$db_user = 'root';
$db_pass = 'tarhush';
$db_name = 'cw';

// 2) Подключаемся к базе
$mysqli = new mysqli($db_host, $db_user, $db_pass, $db_name);
if ($mysqli->connect_error) {
    die('Ошибка подключения (' . $mysqli->connect_errno . ') ' . $mysqli->connect_error);
}
$mysqli->set_charset('utf8');

// 3) Обработка отправки формы
$message = '';
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['id'], $_POST['answer'])) {
    $id = intval($_POST['id']);
    $userAnswer = trim($_POST['answer']);

    // Достаем правильный ответ из БД
    $stmt = $mysqli->prepare("SELECT answer FROM `6klass` WHERE id = ?");
    $stmt->bind_param('i', $id);
    $stmt->execute();
    $stmt->bind_result($correctAnswer);
    if ($stmt->fetch()) {
        // Сравниваем без учёта регистра и лишних пробелов
        if (mb_strtolower($userAnswer) === mb_strtolower(trim($correctAnswer))) {
            $message = '✅ Правильно!';
        } else {
            $message = '❌ Неправильно. Правильный ответ: «' . htmlspecialchars($correctAnswer) . '»';
        }
    } else {
        $message = 'Ошибка: вопрос не найден.';
    }
    $stmt->close();
}

// 4) Берём новый случайный вопрос
$stmt = $mysqli->prepare("SELECT id, problem FROM `6klass` ORDER BY RAND() LIMIT 1");
$stmt->execute();
$stmt->bind_result($questionId, $questionText);
$stmt->fetch();
$stmt->close();

$mysqli->close();
?>
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="utf-8">
    <title>Тест по таблице 6klass</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            padding: 2rem;
            max-width: 600px;
            margin: auto;
        }
        .message {
            margin-bottom: 1.5rem;
            font-size: 1.2rem;
        }
        .question {
            font-size: 1.1rem;
            margin-bottom: .5rem;
        }
        input[type="text"] {
            width: 100%;
            padding: .5rem;
            margin-bottom: .5rem;
            font-size: 1rem;
        }
        button {
            padding: .6rem 1.2rem;
            font-size: 1rem;
        }
    </style>
</head>
<body>
    <?php if ($message): ?>
        <div class="message"><?php echo $message; ?></div>
    <?php endif; ?>

    <form method="post" action="">
        <div class="question"><?php echo htmlspecialchars($questionText); ?></div>
        <input type="hidden" name="id" value="<?php echo $questionId; ?>">
        <input type="text" name="answer" placeholder="Ваш ответ" autocomplete="off" required>
        <button type="submit">Проверить</button>
    </form>
</body>
</html>
