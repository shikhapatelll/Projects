<?php
// Pull in a shared file here so this page can reuse common code.
require_once 'includes/db.php';
require_once 'includes/header.php';
// Open the database connection because this page needs live data.
$conn = getDB();

$allowedSort = ['title', 'releaseyear'];
// Store this in a variable so the next lines stay easier to read.
$allowedDir = ['ASC', 'DESC'];
$sort = $_GET['sort'] ?? 'title';
// Read the submitted/requested value first so I can validate it safely.
$dir = strtoupper($_GET['dir'] ?? 'ASC');
if (!in_array($sort, $allowedSort, true)) { $sort = 'title'; }
// Check this condition first so the right branch runs.
if (!in_array($dir, $allowedDir, true)) { $dir = 'ASC'; }

$sql = "SELECT contentid, title, releaseyear, contenttype, agerating, language, duration FROM content ORDER BY $sort $dir, contentid";
// Store this in a variable so the next lines stay easier to read.
$result = $conn->query($sql);
?>
<section>
<!-- Heading helps the user see what this page or section does. -->
    <h2>Browse Content</h2>
    <form method="get" class="card">
        <div class="grid">
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="sort">Sort by</label>
                <select name="sort" id="sort">
<!-- One choice inside the dropdown menu. -->
                    <option value="title" <?= selected($sort, 'title'); ?>>Title</option>
                    <option value="releaseyear" <?= selected($sort, 'releaseyear'); ?>>Release Year</option>
                </select>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="dir">Order</label>
                <select name="dir" id="dir">
<!-- One choice inside the dropdown menu. -->
                    <option value="ASC" <?= selected($dir, 'ASC'); ?>>Ascending</option>
                    <option value="DESC" <?= selected($dir, 'DESC'); ?>>Descending</option>
                </select>
            </div>
        </div>
<!-- Button submits the form or triggers the main action. -->
        <button type="submit">Apply Sort</button>
    </form>

    <table>
        <tr>
            <th>ID</th>
            <th>Title</th>
            <th>Year</th>
            <th>Type</th>
            <th>Age Rating</th>
            <th>Language</th>
            <th>Duration</th>
            <th>Actions</th>
        </tr>
        <?php while ($row = $result->fetch_assoc()): ?>
            <tr>
                <td><?= h($row['contentid']); ?></td>
                <td><?= h($row['title']); ?></td>
                <td><?= h($row['releaseyear']); ?></td>
                <td><?= h($row['contenttype']); ?></td>
                <td><?= h($row['agerating']); ?></td>
                <td><?= h($row['language']); ?></td>
                <td><?= $row['duration'] === null ? 'N/A' : h((string)$row['duration']) . ' min'; ?></td>
                <td class="actions">
                    <a class="button-link" href="content_details.php?contentid=<?= h($row['contentid']); ?>">Details</a>
                    <a class="button-link secondary" href="update_content.php?contentid=<?= h($row['contentid']); ?>">Update</a>
                    <?php if ($row['contenttype'] === 'tv_series'): ?>
                        <a class="button-link" href="add_episode.php?contentid=<?= h($row['contentid']); ?>">Add Episode</a>
                    <?php endif; ?>
                    <a class="button-link danger" href="delete_content.php?contentid=<?= h($row['contentid']); ?>">Delete</a>
                </td>
            </tr>
        <?php endwhile; ?>
    </table>
</section>
<?php closeDB($conn); require_once 'includes/footer.php'; ?>
