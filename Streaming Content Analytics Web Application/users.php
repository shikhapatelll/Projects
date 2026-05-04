<?php
// Pull in a shared file here so this page can reuse common code.
require_once 'includes/db.php';
require_once 'includes/header.php';
// Open the database connection because this page needs live data.
$conn = getDB();
$error = '';
// Store this in a variable so the next lines stay easier to read.
$usersResult = $conn->query("SELECT userid, name, email FROM users ORDER BY name");
$selectedUser = $_GET['userid'] ?? ($_POST['userid'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
$startDate = $_GET['start_date'] ?? '';
$endDate = $_GET['end_date'] ?? '';
// Store this in a variable so the next lines stay easier to read.
$history = null;

if ($selectedUser !== '' && !ctype_digit((string)$selectedUser)) {
    $error = 'Please choose a valid user.';
    $selectedUser = '';
}
if ($startDate !== '' && !is_valid_date($startDate)) {
    $error = 'Please enter a valid start date.';
    $startDate = '';
}
if ($endDate !== '' && !is_valid_date($endDate)) {
    $error = 'Please enter a valid end date.';
    $endDate = '';
}
if ($startDate !== '' && $endDate !== '' && $startDate > $endDate) {
    $error = 'Start date cannot be after end date.';
}

if (is_post() && isset($_POST['add_watch'])) {
// Read the submitted/requested value first so I can validate it safely.
    $userid = trim($_POST['userid'] ?? '');
    $contentid = trim($_POST['contentid'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $watchDate = trim($_POST['watch_date'] ?? '');
    $duration = trim($_POST['watch_duration'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $completed = isset($_POST['completed']) ? 1 : 0;

    if (!ctype_digit($userid) || (int)$userid <= 0 || !ctype_digit($contentid) || (int)$contentid <= 0 || !is_valid_date($watchDate) || !ctype_digit($duration) || (int)$duration <= 0) {
//keep one error message variable so I can show feedback neatly.
        $error = 'Please enter valid viewing event information.';
    } else {
        $uid = (int)$userid;
        $cid = (int)$contentid;
        $dur = (int)$duration;

        $entityCheck = $conn->prepare("SELECT
                EXISTS(SELECT 1 FROM users WHERE userid = ?) AS user_exists,
                EXISTS(SELECT 1 FROM content WHERE contentid = ?) AS content_exists");
        $entityCheck->bind_param('ii', $uid, $cid);
        $entityCheck->execute();
        $entityStatus = $entityCheck->get_result()->fetch_assoc();
        $entityCheck->close();

        if (!$entityStatus['user_exists'] || !$entityStatus['content_exists']) {
            $error = 'Selected user or content item was not found.';
        } else {
            $duplicateCheck = $conn->prepare("SELECT userid FROM watches WHERE userid = ? AND contentid = ? AND date = ?");
            $duplicateCheck->bind_param('iis', $uid, $cid, $watchDate);
            $duplicateCheck->execute();
            $duplicate = $duplicateCheck->get_result()->fetch_assoc();
            $duplicateCheck->close();

            if ($duplicate) {
                $error = 'That viewing event already exists for this user, content item, and date.';
            } else {
                $stmt = $conn->prepare("INSERT INTO watches (userid, contentid, date, duration, completed) VALUES (?, ?, ?, ?, ?)");
        $stmt->bind_param('iisii', $uid, $cid, $watchDate, $dur, $completed);
// Run the query now that everything is ready.
                if ($stmt->execute()) {
                    $stmt->close();
                    redirect('users.php?userid=' . urlencode($userid) . '&msg=' . urlencode('Viewing event added successfully.'));
                }
//keeping one error message variable so I can show feedback neatly.
                $error = 'Could not insert viewing event: ' . $stmt->error;
                $stmt->close();
            }
        }
    }
}

if ($selectedUser !== '' && ctype_digit((string)$selectedUser)) {
// Store this in a variable so the next lines stay easier to read.
    $sql = "SELECT c.title, w.date, w.duration, w.completed
            FROM watches w
            JOIN content c ON w.contentid = c.contentid
            WHERE w.userid = ?";

    $params = [(int)$selectedUser];
// Store this in a variable so the next lines stay easier to read.
    $types = 'i';

    if ($startDate !== '') {
// Store this in a variable so the next lines stay easier to read.
        $sql .= " AND w.date >= ?";
        $params[] = $startDate;
// Store this in a variable so the next lines stay easier to read.
        $types .= 's';
    }
    if ($endDate !== '') {
// Store this in a variable so the next lines stay easier to read.
        $sql .= " AND w.date <= ?";
        $params[] = $endDate;
// Store this in a variable so the next lines stay easier to read.
        $types .= 's';
    }
    $sql .= " ORDER BY w.date, c.title";

// I use a prepared statement here to keep the SQL safer and cleaner.
    $stmt = $conn->prepare($sql);
    $stmt->bind_param($types, ...$params);
// Run the query now that everything is ready.
    $stmt->execute();
    $history = $stmt->get_result();
    $stmt->close();
}

// Store this in a variable so the next lines stay easier to read.
$contentOptions = $conn->query("SELECT contentid, title FROM content ORDER BY title");
?>
<section>
<!-- Heading helps the user see what this page or section does. -->
    <h2>Users and Viewing Activity</h2>
    <?php if ($error): ?><div class="error"><?= h($error); ?></div><?php endif; ?>

    <div class="grid">
        <div class="card">
            <h3>View Viewing History</h3>
<!-- Form starts here so the user can enter or update information. -->
            <form method="get">
                <label for="userid">User</label>
<!-- Dropdown keeps the choices controlled and easy to select. -->
                <select name="userid" id="userid" required>
                    <option value="">Select a user</option>
                    <?php mysqli_data_seek($usersResult, 0); while ($u = $usersResult->fetch_assoc()): ?>
<!-- One choice inside the dropdown menu. -->
                        <option value="<?= h($u['userid']); ?>" <?= selected($selectedUser, $u['userid']); ?>>
                            <?= h($u['name']); ?> (<?= h($u['email']); ?>)
                        </option>
                    <?php endwhile; ?>
                </select>
                <label for="start_date">Start Date</label>
<!-- Input field where the user types or picks a value. -->
                <input type="date" name="start_date" id="start_date" value="<?= h($startDate); ?>">
                <label for="end_date">End Date</label>
<!-- Input field where the user types or picks a value. -->
                <input type="date" name="end_date" id="end_date" value="<?= h($endDate); ?>">
                <button type="submit">Show History</button>
            </form>
        </div>

        <div class="card">
<!-- Heading helps the user see what this page or section does. -->
            <h3>Add Viewing Event</h3>
            <form method="post">
<!-- Input field where the user types or picks a value. -->
                <input type="hidden" name="add_watch" value="1">
                <label for="post_userid">User</label>
<!-- Dropdown keeps the choices controlled and easy to select. -->
                <select name="userid" id="post_userid" required>
                    <option value="">Select a user</option>
                    <?php mysqli_data_seek($usersResult, 0); while ($u = $usersResult->fetch_assoc()): ?>
<!-- One choice inside the dropdown menu. -->
                        <option value="<?= h($u['userid']); ?>" <?= selected($selectedUser, $u['userid']); ?>>
                            <?= h($u['name']); ?>
                        </option>
                    <?php endwhile; ?>
                </select>
                <label for="contentid">Content</label>
<!-- Dropdown keeps the choices controlled and easy to select. -->
                <select name="contentid" id="contentid" required>
                    <option value="">Select content</option>
                    <?php while ($c = $contentOptions->fetch_assoc()): ?>
<!-- One choice inside the dropdown menu. -->
                        <option value="<?= h($c['contentid']); ?>"><?= h($c['title']); ?> (ID <?= h($c['contentid']); ?>)</option>
                    <?php endwhile; ?>
                </select>
                <label for="watch_date">Watch Date</label>
<!-- Input field where the user types or picks a value. -->
                <input type="date" name="watch_date" id="watch_date" required>
                <label for="watch_duration">Watch Duration (minutes)</label>
<!-- Input field where the user types or picks a value. -->
                <input type="number" name="watch_duration" id="watch_duration" required>
                <label><input type="checkbox" name="completed" value="1"> Completed</label>
<!-- Button submits the form or triggers the main action. -->
                <button type="submit">Add Viewing Event</button>
            </form>
        </div>
    </div>

    <?php if ($history): ?>
        <div class="card">
            <h3>Viewing History</h3>
<!-- Table starts here to display database results clearly. -->
            <table>
                <tr><th>Content Title</th><th>Watch Date</th><th>Watch Duration</th><th>Completed</th></tr>
                <?php while ($row = $history->fetch_assoc()): ?>
                    <tr>
                        <td><?= h($row['title']); ?></td>
                        <td><?= h($row['date']); ?></td>
                        <td><?= h($row['duration']); ?> min</td>
                        <td><?= ((int)$row['completed'] === 1) ? 'Yes' : 'No'; ?></td>
                    </tr>
                <?php endwhile; ?>
            </table>
        </div>
    <?php elseif ($selectedUser !== ''): ?>
        <div class="card"><p>No viewing history found for the selected filters.</p></div>
    <?php endif; ?>
</section>
<?php closeDB($conn); require_once 'includes/footer.php'; ?>
