const char* MAIN_PAGE_HTML = R""""(
	<html>
		<head>
			<title>Lobster R.O.L. Control Panel</title>
			<style>
				* {
				  box-sizing: border-box;
				  font-family: Calibri, Arial, sans-serif;
				  font-size: 20pt;
				  color: rgba(241, 117, 117, 0.867);
				}
				
				button, input[type="text"] {
					background-color: #222230;
					color: rgba(241, 117, 117, 0.867);
					border: 1px solid rgba(241, 117, 117, 0.867);
					font-size: 20pt;
					border-radius: 10px;
					padding-left: 10px;
					padding-right: 10px;
				}
	
				.timeDateStyle{
					background-color: #222230;
					color: rgba(241, 117, 117, 0.867);
					border: 1px solid rgba(241, 117, 117, 0.867);
					font-size: 20pt;
					border-radius: 10px;
					padding-left: 10px;
					padding-right: 10px;
				}
				
			  
				body {
				  margin-left: calc(5%);
				  background-color: #222230;
				}
			  
				h1, h2, h3 {
				  margin-top: 3%;
				  margin-bottom: 5px;
				  font-weight: normal;
				}
			  
				h1, h1 text {
				  font-size: 30pt;
				}
			  
				h2, h2 text {
				  font-size: 25pt;
				}
			  
				h3, h3 text {
				  font-size: 20pt;
				}
			  
				text {
				  font-size: 18pt;
				}
			  
				.setButtonClass {
					background-color: #222230;
					border: 1px solid rgba(241, 117, 117, 0.867);
					color: rgba(241, 117, 117, 0.867);
				}
				.setButtonClass:active {
					background-color:rgba(241, 117, 117, 0.867);
					color: #222230;
				}
	
				.overrideButtonClass {
					background-color: #222230;
					border: 1px solid rgba(241, 117, 117, 0.867);
					color: rgba(241, 117, 117, 0.867);
					border-radius: 10px;
					padding: 10px 20px;
					text-align: center;
					text-decoration: none;
					display: inline-block;
					font-size: 24px;
					min-width: 200px;
				}
				.overrideButtonClass:active {
					background-color:rgba(241, 117, 117, 0.867);
					color: #222230;
				}
	
				.confirmButtonClass {
					background-color: #222230;
					border: 1px solid rgba(241, 117, 117, 0.867);
					color: rgba(241, 117, 117, 0.867);
					border-radius: 10px;
					padding: 10px 20px;
					text-align: center;
					text-decoration: none;
					display: inline-block;
					font-size: 24px;
					min-width: 200px;
				}
				.confirmButtonClass input {
					opacity: 0;
					width: 0;
					height: 0;
				}
				.highlighted {
					background-color: rgba(241, 117, 117, 0.867);
					color:#222230;
				}
				
			  </style>
		</head>
		<body>
			<h1>Lobster R.O.L. Control Panel</h1>
			<text id="uptime"></text>
			<br>
			
	
			<strong>For this device:</strong>
			<br>
			The current timestamp is:
			<strong id="local_timestamp"></strong>
			seconds.
			<br>
			The equivalent date and time is:
			<strong id="local_datetime"></strong>
			<br>
			<br>
			
			<strong>For the Lobster R.O.L.:</strong>
			<br>
			The current timestamp is:
			<strong id="rol_timestamp"></strong>
			seconds.
			<br>
			The equivalent date and time is:
			<strong id="rol_datetime"></strong>
			<br>
			<br>
	
	
			<label for="epochTime">Current time (seconds since 1/1/1970)</label><br>
			<input type="text" step="1" value="0" id="epochTime">
			<button type="button" class="setButtonClass" id="setTime">Sync time</button>
			<br><br>
			
			Release time and date: 
			<br>
			<input type="date" id="releaseDate" class="timeDateStyle">
			<input type="time" id="releaseTime" class="timeDateStyle">
			
			<!-- <input type="text" step="1" value="1670483828" id="releaseTime"> -->
			
			<!-- <script>
				document.getElementById("releaseTime").value = Math.floor(Date.now() / 1000);
			</script> -->
			<button type="button" class="setButtonClass" id="setReleaseTime">Set release time</button>
			<br>
			<br>
			<button id="confirmButton" class="confirmButtonClass">confirm</button>
			<br>
			<br>
			<br>
			<h3>Overrides:</h3>
			<button type="button" id="attachButton" class="overrideButtonClass">Attach</button>
			<button type="button" id="releaseButton" class="overrideButtonClass">Release</button>
	
			<script>
							
				var confirmButtonToggle = document.getElementById("confirmButton");
				var confirmButtonPrevState = false;
				confirmButtonToggle.onclick = function() {
					confirmButtonToggle.classList.toggle("highlighted");
					setConfirmed(confirmButtonToggle.classList.contains("highlighted"));
					if (confirmButtonToggle.classList.contains("highlighted")) {
						confirmButtonToggle.innerText = "Confirmed!";
					} else {
						confirmButtonToggle.innerText = "Confirm";
					}
				}
	
				let epochTime = document.querySelector("#epochTime");
				let releaseTime = document.querySelector("#releaseTime");
				let rtcTime = document.getElementById("rol_timestamp");
				// let confirmCheck = document.querySelector("#confirmButton");
	
	
				document.getElementById("setTime").onclick = e => {
					setEpochTime(epochTime.value);
				};
				document.getElementById("setReleaseTime").onclick = e => {
					setReleaseTime();
				};
				
				document.getElementById("attachButton").onclick = e => {
					attachButton();
				};
				
				document.getElementById("releaseButton").onclick = e => {
					releaseButton();
				};
	
				// confirmCheck.oninput = e => {
				// 	setConfirmed(confirmCheck.checked);
				// };
				
				fetchValues();
				updatePageTime();
				setInterval(fetchValues, 1000);
				setInterval(updatePageTime, 1000);
	
				function fetchValues() {
	
					setTimeout(() => {
						fetch("/rtcTime", {
							method: 'GET'
						}).then(v => v.text()).then(v => rtcTime.innerHTML = parseInt(v));
						updateDeviceTime(rtcTime.innerHTML);
					}, 1000);
	
					// setTimeout(() => {
					// 	fetch("/epochTime", {
					// 		method: 'GET'
					// 	}).then(v => v.text()).then(v => epochTime.value = parseInt(v));
					// }, 1000);
					
					// setTimeout(() => {
					// 	fetch("/releaseTime", {
					// 		method: 'GET'
					// 	}).then(v => v.text()).then(v => releaseTime.value = parseInt(v));
					// }, 1000);
	
					setTimeout(() => {
						fetch("/confirmButton", {
						method: 'GET'
						}).then(v => v.text()).then(v => confirmButtonPrevState = v == 1);
						
						if ((confirmButtonToggle.classList.contains("highlighted") != confirmButtonPrevState)){
							confirmButtonToggle.classList.toggle("highlighted");
						}
						if(confirmButtonToggle.classList.contains("highlighted")){
							confirmButtonToggle.innerText = "Confirmed!";
						} else {
							confirmButtonToggle.innerText = "Confirm";
						}
					}, 1000);
				}
	
				function updateDeviceTime(v){
					var rol_datetime = new Date(v * 1000);
					document.getElementById('rol_datetime').innerHTML = rol_datetime;
				}
				function updatePageTime(){
					var local_timestamp = Date.now();
					var local_timestamp_s = Math.floor(local_timestamp / 1000);
					document.getElementById('local_timestamp').innerHTML = local_timestamp_s;
					var local_datetime = new Date(local_timestamp);
					document.getElementById('local_datetime').innerHTML = local_datetime;
					document.getElementById("epochTime").value = local_timestamp_s;
				}
	
				function setEpochTime(v) {
					return fetch("/epochTime?value=" + v, {
						method: 'POST',
						mode: 'cors',
						cache: 'no-cache',
						credentials: 'same-origin',
						headers: {
						'Content-Type': 'application/json'
						},
						redirect: 'follow',
						referrerPolicy: 'no-referrer',
						body: "{}"
					});
				}
				
				function setReleaseTime() {
					// Get the date and time from the input fields
					var date = document.getElementById('releaseDate').value;
					var time = document.getElementById('releaseTime').value;
					
					// Convert the date and time to a Date object
					var dateTime = new Date(date + ' ' + time);
					
					// Get the unix timestamp from the Date object
					var timestamp = dateTime.getTime() / 1000;
					
					// // Update the timestamp in the page
					// document.getElementById('timestamp').innerHTML = timestamp;
					
					return fetch("/releaseTime?value=" + timestamp, {
						method: 'POST',
						mode: 'cors',
						cache: 'no-cache',
						credentials: 'same-origin',
						headers: {
						'Content-Type': 'application/json'
						},
						redirect: 'follow',
						referrerPolicy: 'no-referrer',
						body: "{}"
					});
				}
	
				function setConfirmed(v) {
					return fetch("/confirmButton?value=" + (v == true), { // explicitly coerse to bool
						method: 'POST',
						mode: 'cors',
						cache: 'no-cache',
						credentials: 'same-origin',
						headers: {
						'Content-Type': 'application/json'
						},
						redirect: 'follow',
						referrerPolicy: 'no-referrer',
						body: "{}"
					});
				}
				
				function attachButton() {
					return fetch("/attachButton?value=" + true, { // explicitly coerse to bool
						method: 'POST',
						mode: 'cors',
						cache: 'no-cache',
						credentials: 'same-origin',
						headers: {
						'Content-Type': 'application/json'
						},
						redirect: 'follow',
						referrerPolicy: 'no-referrer',
						body: "{}"
					});
				}
				
				function releaseButton() {
					return fetch("/releaseButton?value=" + true, { // explicitly coerse to bool
						method: 'POST',
						mode: 'cors',
						cache: 'no-cache',
						credentials: 'same-origin',
						headers: {
						'Content-Type': 'application/json'
						},
						redirect: 'follow',
						referrerPolicy: 'no-referrer',
						body: "{}"
					});
				}
	
			</script>
		</body>
	</html>
)"""";