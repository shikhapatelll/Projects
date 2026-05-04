<?php require_once __DIR__ . '/functions.php'; ?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CS3319 Assignment 3 - Streaming Platform</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
<div class="wrapper">
    <header>
<!-- Heading helps the user see what this page or section does. -->
        <h1>Streaming Platform Manager</h1>
        <p>CS3319 Assignment 3</p>
    </header>
    <nav>
        <a href="mainmenu.php">Home</a>
        <a href="content.php">Browse Content</a>
        <a href="add_content.php">Add Content</a>
        <a href="users.php">Users & Viewing</a>
        <a href="ratings.php">Ratings</a>
        <a href="plans.php">Subscription Plans</a>
    </nav>
    <main>
        <?= flash_message(); ?>
