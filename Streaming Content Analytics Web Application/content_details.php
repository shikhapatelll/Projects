<?php
// Pull in a shared file here so this page can reuse common code.
require_once 'includes/db.php';
require_once 'includes/header.php';
// Open the database connection because this page needs live data.
$conn = getDB();
$contentid = $_GET['contentid'] ?? '';
// keeping one error message variable so I can show feedback neatly.
$error = '';
$details = null;
// Store this in a variable so the next lines stay easier to read.
$episodes = null;

if ($contentid === '' || !ctype_digit($contentid)) {
// keeping one error message variable so I can show feedback neatly.
    $error = 'Invalid content id.';
} else {
    $stmt = $conn->prepare(
        "SELECT c.contentid, c.title, c.contenttype, c.releaseyear, c.agerating, c.language, c.duration,
                (SELECT ROUND(AVG(r.score), 2) FROM rates r WHERE r.contentid = c.contentid) AS avg_rating,
                (SELECT COUNT(*) FROM watches w WHERE w.contentid = c.contentid) AS total_views
         FROM content c
         WHERE c.contentid = ?"
    );
// Match the PHP values to the placeholders before executing the query.
    $stmt->bind_param('i', $contentid);
    $stmt->execute();
// Read one row at a time into an associative array.
    $details = $stmt->get_result()->fetch_assoc();
    $stmt->close();

    if (!$details) {
// keep one error message variable so I can show feedback neatly.
        $error = 'Content item not found.';
    } elseif ($details['contenttype'] === 'tv_series') {
        $stmt = $conn->prepare("SELECT episodenumber, title, duration, releasedate FROM episodes WHERE contentid = ? ORDER BY episodenumber");
// Match the PHP values to the placeholders before executing the query.
        $stmt->bind_param('i', $contentid);
        $stmt->execute();
// Store this in a variable so the next lines stay easier to read.
        $episodes = $stmt->get_result();
        $stmt->close();
    }
}
?>
<section>
<!-- Heading helps the user see what this page or section does. -->
    <h2>Content Details</h2>
    <?php if ($error): ?>
        <div class="error"><?= h($error); ?></div>
    <?php else: ?>
        <div class="card">
            <p><strong>Content ID:</strong> <?= h($details['contentid']); ?></p>
            <p><strong>Title:</strong> <?= h($details['title']); ?></p>
            <p><strong>Type:</strong> <?= h($details['contenttype']); ?></p>
            <p><strong>Release Year:</strong> <?= h($details['releaseyear']); ?></p>
            <p><strong>Age Rating:</strong> <?= h($details['agerating']); ?></p>
            <p><strong>Language:</strong> <?= h($details['language']); ?></p>
            <p><strong>Duration:</strong> <?= $details['duration'] === null ? 'N/A (TV series)' : h((string)$details['duration']) . ' minutes'; ?></p>
            <p><strong>Average Rating:</strong> <?= $details['avg_rating'] === null ? 'No ratings yet' : h((string)$details['avg_rating']); ?></p>
            <p><strong>Total Views:</strong> <?= h((string)$details['total_views']); ?></p>
        </div>
        <?php if ($details['contenttype'] === 'tv_series'): ?>
            <div class="card">
                <h3>Episodes</h3>
<!-- Table starts here to display database results clearly. -->
                <table>
                    <tr><th>Episode #</th><th>Title</th><th>Duration</th><th>Release Date</th></tr>
                    <?php while ($ep = $episodes->fetch_assoc()): ?>
                        <tr>
                            <td><?= h($ep['episodenumber']); ?></td>
                            <td><?= h($ep['title']); ?></td>
                            <td><?= h($ep['duration']); ?> min</td>
                            <td><?= h($ep['releasedate']); ?></td>
                        </tr>
                    <?php endwhile; ?>
                </table>
            </div>
        <?php endif; ?>
    <?php endif; ?>
    <a class="button-link secondary" href="content.php">Back to Content</a>
</section>
<?php closeDB($conn); require_once 'includes/footer.php'; ?>
