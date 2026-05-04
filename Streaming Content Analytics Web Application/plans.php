<?php
// Pull in a shared file here so this page can reuse common code.
require_once 'includes/db.php';
require_once 'includes/header.php';
// Open the database connection because this page needs live data.
$conn = getDB();
$error = '';

// Only run this block after the user submits the form.
if (is_post()) {
    $planid = trim($_POST['planid'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $monthlyprice = trim($_POST['monthlyprice'] ?? '');
    if (!ctype_digit($planid) || (int)$planid <= 0 || !is_numeric($monthlyprice) || (float)$monthlyprice < 0) {
// keeping one error message variable so I can show feedback neatly.
        $error = 'Please enter a valid plan and price.';
    } else {
        $check = $conn->prepare("SELECT planid FROM subscription_plans WHERE planid = ?");
        $pid = (int)$planid;
        $check->bind_param('i', $pid);
        $check->execute();
        $exists = $check->get_result()->fetch_assoc();
        $check->close();

        if (!$exists) {
            $error = 'Selected plan was not found.';
        } else {
            $stmt = $conn->prepare("UPDATE subscription_plans SET monthlyprice = ? WHERE planid = ?");
//Storing this in a variable so the next lines stay easier to read.
        $pid = (int)$planid;
        $price = (float)$monthlyprice;
// Match the PHP values to the placeholders before executing the query.
        $stmt->bind_param('di', $price, $pid);
        if ($stmt->execute()) {
            $stmt->close();
// After success, send the user to the next page with a friendly message.
            redirect('plans.php?msg=' . urlencode('Monthly price updated successfully.'));
        }
            $error = 'Update failed: ' . $stmt->error;
            $stmt->close();
        }
    }
}

// Storing this in a variable so the next lines stay easier to read.
$planSummary = $conn->query(
    "SELECT sp.planid, sp.name, sp.monthlyprice, sp.maxvideoquality, sp.maxconcurrentstreams,
            COUNT(u.userid) AS subscriber_count
     FROM subscription_plans sp
     LEFT JOIN users u ON sp.planid = u.planid
     GROUP BY sp.planid, sp.name, sp.monthlyprice, sp.maxvideoquality, sp.maxconcurrentstreams
     ORDER BY sp.planid"
);
?>
<section>
<!-- Heading helps the user see what this page or section does. -->
    <h2>Subscription Plans</h2>
    <?php if ($error): ?><div class="error"><?= h($error); ?></div><?php endif; ?>
    <div class="card">
        <h3>Plan Summary</h3>
<!-- Table starts here to display database results clearly. -->
        <table>
            <tr>
                <th>Plan ID</th><th>Name</th><th>Monthly Price</th><th>Max Video Quality</th><th>Max Concurrent Streams</th><th>Subscribers</th>
            </tr>
            <?php while ($row = $planSummary->fetch_assoc()): ?>
                <tr>
                    <td><?= h($row['planid']); ?></td>
                    <td><?= h($row['name']); ?></td>
                    <td>$<?= h(number_format((float)$row['monthlyprice'], 2)); ?></td>
                    <td><?= h($row['maxvideoquality']); ?></td>
                    <td><?= h($row['maxconcurrentstreams']); ?></td>
                    <td><?= h($row['subscriber_count']); ?></td>
                </tr>
            <?php endwhile; ?>
        </table>
    </div>

    <div class="card">
        <h3>Update Monthly Price</h3>
        <p class="small">Deletion of plans with subscribed users is intentionally not offered. Your Assignment 2 foreign key already prevents invalid deletion.</p>
<!-- Form starts here so the user can enter or update information. -->
        <form method="post">
            <label for="planid">Plan</label>
<!-- Dropdown keeps the choices controlled and easy to select. -->
            <select name="planid" id="planid" required>
                <option value="">Select a plan</option>
                <?php
// Storing this in a variable so the next lines stay easier to read.
                $plans = $conn->query("SELECT planid, name, monthlyprice FROM subscription_plans ORDER BY planid");
                while ($p = $plans->fetch_assoc()): ?>
<!-- One choice inside the dropdown menu. -->
                    <option value="<?= h($p['planid']); ?>"><?= h($p['name']); ?> (current $<?= h(number_format((float)$p['monthlyprice'], 2)); ?>)</option>
                <?php endwhile; ?>
            </select>
            <label for="monthlyprice">New Monthly Price</label>
<!-- Input field where the user types or picks a value. -->
            <input type="number" step="0.01" min="0" name="monthlyprice" id="monthlyprice" required>
            <button type="submit">Update Price</button>
        </form>
    </div>
</section>
<?php closeDB($conn); require_once 'includes/footer.php'; ?>
