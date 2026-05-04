<?php require_once 'includes/header.php'; ?>
<!-- Main section of this page starts here. -->
<section>
    <h2>Welcome</h2>
    <p>This web application manages the streaming platform database from Assignment 2.</p>
    <div class="grid">
        <div class="card">
<!-- Heading helps the user see what this page or section does. -->
            <h3>Content Browsing and Analytics</h3>
            <p>List all content, sort it, and view details, ratings, and views.</p>
            <a class="button-link" href="content.php">Open Content</a>
        </div>
        <div class="card">
            <h3>Managing Content</h3>
            <p>Add movies or TV series, then add episodes and update fields.</p>
            <a class="button-link" href="add_content.php">Add Content</a>
        </div>
        <div class="card">
<!-- Heading helps the user see what this page or section does. -->
            <h3>Users and Viewing Activity</h3>
            <p>See watch history, filter by date range, and insert viewing events.</p>
            <a class="button-link" href="users.php">Open Users</a>
        </div>
        <div class="card">
            <h3>Ratings</h3>
            <p>Add a rating or update an existing one if the user already rated that content.</p>
            <a class="button-link" href="ratings.php">Manage Ratings</a>
        </div>
        <div class="card">
<!-- Heading helps the user see what this page or section does. -->
            <h3>Subscription Plans</h3>
            <p>View plan counts and update plan monthly prices.</p>
            <a class="button-link" href="plans.php">View Plans</a>
        </div>
    </div>
</section>
<?php require_once 'includes/footer.php'; ?>
