<?php
// Database connection parameters
$host     = 'localhost';
$username = 'root';
$password = 'tarhush';
$database = 'cw';

// Create a new connection
$db = new mysqli($host, $username, $password, $database);

// Check connection
if ($db->connect_error) {
    die("Connection failed: " . $db->connect_error);
}

$resultMessage = "";
// If the form is submitted, then process the answer
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Retrieve the problem ID and user's answer from form input
    $problemId = isset($_POST['problem_id']) ? intval($_POST['problem_id']) : 0;
    $userAnswer = isset($_POST['user_answer']) ? trim($_POST['user_answer']) : '';

    // Prepare a statement to get the correct answer from the 6klass table
    $stmt = $db->prepare("SELECT answer FROM `6klass` WHERE id = ?");
    $stmt->bind_param("i", $problemId);
    $stmt->execute();
    $stmt->bind_result($correctAnswer);
    $stmt->fetch();
    $stmt->close();

    // Compare the user's answer with the correct answer
    if ($userAnswer === (string)$correctAnswer) {
        $resultMessage = "<p class='result'>Верно!</p>";
    } else {
        $resultMessage = "<p class='error'>Неправильно! Правильный ответ: " . htmlspecialchars($correctAnswer) . "</p>";
    }
    
    // Provide a link to try another problem
    $resultMessage .= '<p><a href="' . htmlspecialchars($_SERVER['PHP_SELF']) . '">Попробовать ещё</a></p>';
} else {
    // Otherwise, select a random problem from the 6klass table
    $query = "SELECT id, problem FROM `6klass` ORDER BY RAND() LIMIT 1";
    $result = $db->query($query);
    if ($result && $result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $problemId   = $row['id'];
        $problemText = $row['problem'];
    } else {
        die("Нет доступных задач");
    }
    $result->free();
}
$db->close();
?>
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <title>Генератор уравнений - Уровень 6Класс</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            padding: 20px;
            background-color: #f0f0f0;
            text-align: center;
        }
        input[type="text"] {
            padding: 5px;
            font-size: 16px;
            width: 300px;
        }
        button {
            padding: 5px 10px;
            font-size: 16px;
        }
        .result {
            color: green;
            font-weight: bold;
        }
        .error {
            color: red;
            font-weight: bold;
        }
        a {
            text-decoration: none;
            font-size: 16px;
        }
    </style>
</head>
<body>
    <h1>Математическая игра (Уровень 6Класс)</h1>
<?php if ($_SERVER['REQUEST_METHOD'] === 'POST'): ?>
    <?php echo $resultMessage; ?>
<?php else: ?>
    <p><strong>Решите задачу:</strong></p>
    <p><?php echo htmlspecialchars($problemText); ?></p>
    <form method="post" action="<?php echo htmlspecialchars($_SERVER['PHP_SELF']); ?>">
        <input type="hidden" name="problem_id" value="<?php echo $problemId; ?>">
        <input type="text" name="user_answer" placeholder="Ваш ответ" required>
        <button type="submit">Ответить</button>
    </form>
<?php endif; ?>
</body>
</html>
