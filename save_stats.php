<?php
// ÐŸÐ°Ñ€Ð°Ð¼ÐµÑ‚Ñ€Ñ‹ Ð¿Ð¾Ð´ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ñ Ðº Ð±Ð°Ð·Ðµ Ð´Ð°Ð½Ð½Ñ‹Ñ…
$servername = "localhost";
$username   = "root";
$password   = "tarhush";
$dbname     = "cw";

// Ð¡Ð¾Ð·Ð´Ð°Ð½Ð¸Ðµ Ð¿Ð¾Ð´ÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ñ
$conn = new mysqli($servername, $username, $password, $dbname);

// ÐŸÑ€Ð¾Ð²ÐµÑ€ÐºÐ° ÑÐ¾ÐµÐ´Ð¸Ð½ÐµÐ½Ð¸Ñ
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// ÐŸÑ€Ð¾Ð²ÐµÑ€ÑÐµÐ¼, Ñ‡Ñ‚Ð¾ Ð²ÑÐµ Ð½ÐµÐ¾Ð±Ñ…Ð¾Ð´Ð¸Ð¼Ñ‹Ðµ Ð¿Ð¾Ð»Ñ Ð¿ÐµÑ€ÐµÐ´Ð°Ð½Ñ‹ Ñ‡ÐµÑ€ÐµÐ· POST
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
    // ÐŸÐ¾Ð»ÑƒÑ‡ÐµÐ½Ð¸Ðµ Ð´Ð°Ð½Ð½Ñ‹Ñ… Ð¸ Ð¿Ñ€ÐµÐ¾Ð±Ñ€Ð°Ð·Ð¾Ð²Ð°Ð½Ð¸Ðµ Ð² Ð½ÑƒÐ¶Ð½Ñ‹Ðµ Ñ‚Ð¸Ð¿Ñ‹
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
    
    // ÐŸÐ¾Ð´Ð³Ð¾Ñ‚Ð¾Ð²ÐºÐ° SQL-Ð·Ð°Ð¿Ñ€Ð¾ÑÐ° Ñ Ð´Ð¾Ð±Ð°Ð²Ð»ÐµÐ½Ð¸ÐµÐ¼ Ð¸Ð¼ÐµÐ½Ð¸
    $stmt = $conn->prepare("INSERT INTO game_stats 
        (difficulty, best_correct, worst_correct, avg_correct, best_incorrect, worst_incorrect, avg_incorrect, correct_count, incorrect_count, history, name, created_at) 
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, NOW())");
    
    if ($stmt === false) {
        die("ÐžÑˆÐ¸Ð±ÐºÐ° Ð¿Ð¾Ð´Ð³Ð¾Ñ‚Ð¾Ð²ÐºÐ¸ Ð·Ð°Ð¿Ñ€Ð¾ÑÐ°: " . $conn->error);
    }
    
    // ÐŸÑ€Ð¸Ð²ÑÐ·ÐºÐ° Ð¿Ð°Ñ€Ð°Ð¼ÐµÑ‚Ñ€Ð¾Ð²: 
    // "i" - difficulty,
    // 6 Ð¿Ð°Ñ€Ð°Ð¼ÐµÑ‚Ñ€Ð¾Ð² Ñ Ð¿Ð»Ð°Ð²Ð°ÑŽÑ‰ÐµÐ¹ Ñ‚Ð¾Ñ‡ÐºÐ¾Ð¹ ("d"),
    // 2 Ñ†ÐµÐ»Ñ‹Ñ… Ñ‡Ð¸ÑÐ»Ð° ("i"), 2 ÑÑ‚Ñ€Ð¾ÐºÐ¸ ("s")
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
        echo "Ð¡Ñ‚Ð°Ñ‚Ð¸ÑÑ‚Ð¸ÐºÐ° Ð¸Ð³Ñ€Ñ‹ ÑƒÑÐ¿ÐµÑˆÐ½Ð¾ ÑÐ¾Ñ…Ñ€Ð°Ð½ÐµÐ½Ð°!";
    } else {
        echo "ÐžÑˆÐ¸Ð±ÐºÐ°: " . $stmt->error;
    }
    
    $stmt->close();
} else {
    echo "ÐÐµ Ð¿ÐµÑ€ÐµÐ´Ð°Ð½Ñ‹ Ð½ÐµÐ¾Ð±Ñ…Ð¾Ð´Ð¸Ð¼Ñ‹Ðµ Ð´Ð°Ð½Ð½Ñ‹Ðµ.";
}

$conn->close();
?>
