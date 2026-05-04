<?php
// Pull in a shared file here so this page can reuse common code.
require_once __DIR__ . '/config.php';

function getDB(): mysqli {
// Store this in a variable so the next lines stay easier to read.
    $conn = new mysqli(DB_HOST, DB_USER, DB_PASS, DB_NAME);
    if ($conn->connect_error) {
        die('Database connection failed: ' . htmlspecialchars($conn->connect_error));
    }
    $conn->set_charset('utf8mb4');
// Send the result back to the code that called this function.
    return $conn;
}

function closeDB(?mysqli $conn): void {
// Check this condition first so the right branch runs.
    if ($conn instanceof mysqli) {
        $conn->close();
    }
}
?>
