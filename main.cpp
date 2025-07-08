#include <windows.h>
#include <mmsystem.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.14159265358979323846

//Animation & Scene Control
float waveOffset = 0.0f;
float cloudOffset1 = 0.0f;
float cloudOffset2 = -0.5f;
bool isNightTransition = false; // Used to trigger day/night

//Fighter Jet Animation Control
float jetPosX = 2.0f;
float jetPosY = 1.2f;
float jetAngle = 0.0f;
float jetSpeed = 0.0f;
int jetLandingState = 0;


float landingApproachStartPosX = 0.0f;
float landingApproachStartPosY = 0.0f;

float landingTargetX = -0.3f;
float landingTargetY = -0.075f;
float landingCurrentTime = 0.0f;
float landingDuration = 150.0f; // Frames for a smoother
float jetParkedX = 0.2f;

//Day/Night Cycle
float skyRed = 0.4f, skyGreen = 0.7f, skyBlue = 1.0f;
float sunAlpha = 1.0f; // For fading the sun
float starAlpha = 0.0f; // For fading in stars
float lightGlow = 0.0f; // For Helicarrier lights
float sunMoonAngle = 90.0f; // Sun & Moon Angle

//Searchlight ---
float searchlightAngle = 0.0f;

// Draws a filled circle
void drawCircle(float cx, float cy, float r, float red, float green, float blue, float alpha) {
    glColor4f(red, green, blue, alpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 50; ++i) {
        float angle = 2.0f * PI * i / 50;
        glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
    }
    glEnd();
}

// Draws a cloud multiple overlapping circles.
void drawCloud(float x, float y) {
    drawCircle(x, y, 0.07f, 1.0f, 1.0f, 1.0f, 0.8f);
    drawCircle(x + 0.06f, y + 0.02f, 0.07f, 1.0f, 1.0f, 1.0f, 0.8f);
    drawCircle(x - 0.06f, y + 0.02f, 0.07f, 1.0f, 1.0f, 1.0f, 0.8f);
}

// Draws a scattering of white stars for the night sky.
void drawStars() {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    glColor4f(1.0f, 1.0f, 1.0f, starAlpha); // Stars fade
    for(int i = 0; i < 100; i++) {
        //star positions
        float x = (float) ( (i * 1234) % 2000 - 1000) / 1000.0f;
        float y = (float) ( (i * 5678) % 1000) / 1000.0f;
        if(y > 0.1) glVertex2f(x, y); // Only draw stars in the upper part of the sky
    }
    glEnd();
    glPointSize(1.0f);
}


void drawGradientSky() {
    drawStars(); // Draw stars first so they appear behind the gradient
    glBegin(GL_QUADS);

    glColor4f(skyRed * 0.7f, skyGreen * 0.7f, skyBlue * 0.9f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);

    glColor4f(skyRed, skyGreen, skyBlue, 1.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(-1.0f, 0.0f);
    glEnd();
}

void drawSunAndMoon() {

    float celestialX = 1.0f * cos(sunMoonAngle * PI / 180.0f);
    float celestialY = 0.8f * sin(sunMoonAngle * PI / 180.0f);

    if (sunAlpha > 0.5f) {
        drawCircle(celestialX, celestialY, 0.1f, 1.0f, 0.9f, 0.0f, sunAlpha); // Sun
    } else {
        drawCircle(celestialX, celestialY, 0.08f, 0.9f, 0.9f, 0.9f, starAlpha); // Moon
    }
}

// Draws the ocean wave
void drawWaves() {

    glColor3f(0.0f, 0.4f * (sunAlpha * 0.7f + 0.3f), 0.8f * (sunAlpha * 0.7f + 0.3f));
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(-1.0f, 0.0f);
    glEnd();

    // Wave ripples
    glColor4f(0.2f, 0.6f, 1.0f, sunAlpha * 0.5f + 0.5f);
    glBegin(GL_TRIANGLE_STRIP);
    for (float x = -1.0f; x <= 1.05f; x += 0.02f) {
        float y = 0.02f * sin(10.0f * x + waveOffset);
        glVertex2f(x, 0.0f + y);
        glVertex2f(x, -0.05f + y);
    }
    glEnd();
}

//searchlight beam rotate
void drawRotatingSearchlight(float cx, float cy) {
    float beamLength = 1.5f;
    float angleRad = searchlightAngle * PI / 180.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 0.8f, 0.0f);
    glColor4f(1.0f, 1.0f, 0.8f, 0.2f * lightGlow);
    glVertex2f(cx, cy);
    glVertex2f(cx + beamLength * cos(angleRad - 0.02f), cy + beamLength * sin(angleRad - 0.02f));
    glVertex2f(cx + beamLength * cos(angleRad + 0.02f), cy + beamLength * sin(angleRad + 0.02f));
    glEnd();
    glDisable(GL_BLEND);
}


// Draws the fighter jet
void drawFighterJet() {

    glBegin(GL_POLYGON);
    glColor3f(0.1451f, 0.2235f, 0.2157f);
    glVertex2f(-0.875f, 0.267f);
    glVertex2f(-0.843f, 0.228f);
    glVertex2f(-0.8f, 0.2f);
    glVertex2f(-0.6f, 0.2f);
    glVertex2f(-0.591f, 0.207f);
    glVertex2f(-0.5960f, 0.2147f);
    glVertex2f(-0.4558f, 0.252f);
    glEnd();

    // Main fuselage and wing section
    glBegin(GL_POLYGON);
    glColor3f(0.4902f, 0.5059f, 0.5020f);
    glVertex2f(-0.875f, 0.267f);
    glVertex2f(-0.4558f, 0.252f);
    glVertex2f(-0.5655f, 0.3049f);
    glVertex2f(-0.5684f, 0.3083f);
    glVertex2f(-0.5721f, 0.3120f);
    glVertex2f(-0.5760f, 0.3161f);
    glVertex2f(-0.58f,  0.32f);
    glVertex2f(-0.5843f, 0.3240f);
    glVertex2f(-0.5882f, 0.3276f);
    glVertex2f(-0.5918f,  0.3310f);
    glVertex2f(-0.5966f, 0.3349f);
    glVertex2f(-0.6017f, 0.3372f);
    glVertex2f(-0.6082f, 0.3397f);
    glVertex2f(-0.6135f, 0.3391f);
    glVertex2f(-0.6204f, 0.3385f);
    glVertex2f(-0.6263f, 0.3379f);
    glVertex2f(-0.6322f, 0.3361f);
    glVertex2f(-0.6388f, 0.3338f);
    glVertex2f(-0.6445f, 0.3316f);
    glVertex2f(-0.6509f, 0.3283f);
    glVertex2f(-0.6520f, 0.32810f);
    glVertex2f(-0.7329f, 0.3134f);
    glVertex2f(-0.7850f, 0.3281f);
    glVertex2f(-0.846f, 0.315f);
    glVertex2f(-0.8744f, 0.3240f);
    glVertex2f(-0.8849f, 0.3227f);
    glVertex2f(-0.8944f, 0.3171f);
    glVertex2f(-0.8949f, 0.2735f);
    glVertex2f(-0.8852f, 0.2674f);
    glEnd();

    // Tail fin
    glBegin(GL_POLYGON);
    glColor3f(0.6416f, 0.6573f, 0.6573f);
    glVertex2f(-0.785f, 0.3281f);
    glVertex2f(-0.8286f, 0.4231f);
    glVertex2f(-0.8497f, 0.4231f);
    glVertex2f(-0.8750f,  0.3556f);
    glVertex2f(-0.8744f, 0.324f);
    glVertex2f(-0.846f, 0.315f);
    glEnd();

    // Canopy/cockpit area
    glBegin(GL_POLYGON);
    glColor3f(0.7416f, 0.7573f, 0.7573f);
    glVertex2f(-0.5655f, 0.3049f);
    glVertex2f(-0.5684f, 0.3083f);
    glVertex2f(-0.5721f, 0.3120f);
    glVertex2f(-0.5760f, 0.3161f);
    glVertex2f(-0.58f,  0.32f);
    glVertex2f(-0.5843f, 0.3240f);
    glVertex2f(-0.5882f, 0.3276f);
    glVertex2f(-0.5918f,  0.3310f);
    glVertex2f(-0.5966f, 0.3349f);
    glVertex2f(-0.6017f, 0.3372f);
    glVertex2f(-0.6082f, 0.3397f);
    glVertex2f(-0.6135f, 0.3391f);
    glVertex2f(-0.6204f, 0.3385f);
    glVertex2f(-0.6263f, 0.3379f);
    glVertex2f(-0.6322f, 0.3361f);
    glVertex2f(-0.6388f, 0.3338f);
    glVertex2f(-0.6445f, 0.3316f);
    glVertex2f(-0.6509f, 0.3283f);
    glVertex2f(-0.6520f, 0.32810f);
    glVertex2f(-0.7329f, 0.3134f);
    glVertex2f(-0.7850f, 0.3281f);
    glVertex2f(-0.846f, 0.315f);
    glVertex2f(-0.8744f, 0.3240f);
    glVertex2f(-0.8849f, 0.3227f);
    glVertex2f(-0.8944f, 0.3171f);
    glVertex2f(-0.8949f, 0.2735f);
    glVertex2f(-0.8852f, 0.2674f);
    glEnd();
}

// Draws the Helicarrier
void drawHelicarrier() {

    glColor3f(0.18f, 0.18f, 0.18f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.8f, -0.2f);
    glVertex2f(-0.7f, 0.05f);
    glVertex2f(0.7f, 0.05f);
    glVertex2f(0.8f, -0.2f);
    glVertex2f(0.7f, -0.25f);
    glVertex2f(-0.7f, -0.25f);
    glEnd();


    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.7f, -0.15f);
    glVertex2f(-0.6f, 0.02f);
    glVertex2f(0.6f, 0.02f);
    glVertex2f(0.7f, -0.15f);
    glVertex2f(0.6f, -0.2f);
    glVertex2f(-0.6f, -0.2f);
    glEnd();

    // Runway
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-0.4f, 0.0f);
    glVertex2f(0.4f, 0.0f);
    glVertex2f(0.4f, -0.15f);
    glVertex2f(-0.4f, -0.15f);
    glEnd();

    // Runway edge lights
    glColor4f(1.0f, 0.8f, 0.8f, lightGlow);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(-0.4f, 0.0f);
    glVertex2f(0.4f, 0.0f);
    glVertex2f(0.4f, -0.15f);
    glVertex2f(-0.4f, -0.15f);
    glEnd();
    glPointSize(1.0f);

    // Center line
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(-0.4f, -0.075f);
    glVertex2f(0.4f, -0.075f);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    // Control Tower
    glColor3f(0.35f, 0.35f, 0.35f);
    glBegin(GL_POLYGON);
    glVertex2f(0.15f, 0.05f);
    glVertex2f(0.35f, 0.05f);
    glVertex2f(0.32f, 0.2f);
    glVertex2f(0.18f, 0.2f);
    glEnd();

    // Tower top
    glColor3f(0.45f, 0.45f, 0.45f);
    glBegin(GL_POLYGON);
    glVertex2f(0.17f, 0.18f);
    glVertex2f(0.33f, 0.18f);
    glVertex2f(0.3f, 0.22f);
    glVertex2f(0.2f, 0.22f);
    glEnd();


    glColor4f(0.9f, 1.0f, 1.0f, 0.3f + lightGlow * 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0.20f, 0.1f);
    glVertex2f(0.30f, 0.1f);
    glVertex2f(0.28f, 0.16f);
    glVertex2f(0.22f, 0.16f);
    glEnd();

    // Side thrusters
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(-0.85f, -0.1f);
    glVertex2f(-0.65f, -0.1f);
    glVertex2f(-0.65f, -0.22f);
    glVertex2f(-0.85f, -0.22f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(0.65f, -0.1f);
    glVertex2f(0.85f, -0.1f);
    glVertex2f(0.85f, -0.22f);
    glVertex2f(0.65f, -0.22f);
    glEnd();

    // Searchlights
    drawRotatingSearchlight(-0.5f, -0.2f);
    drawRotatingSearchlight(0.5f, -0.2f);

    glColor4f(0.9f, 0.9f, 0.9f, 0.8f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);

    glVertex2f(-0.6f, 0.0f);
    glVertex2f(-0.45f, 0.0f);
    glVertex2f(-0.6f, -0.05f);
    glVertex2f(-0.45f, -0.05f);

    glVertex2f(0.45f, 0.0f);
    glVertex2f(0.6f, 0.0f);
    glVertex2f(0.45f, -0.05f);
    glVertex2f(0.6f, -0.05f);
    glEnd();
    glLineWidth(1.0f);


    drawCircle(0.0f, -0.1f, 0.03f, 0.9f, 0.9f, 0.9f, 0.8f); // White circle lower deck
    // Helipad H
    glColor4f(0.1f, 0.1f, 0.1f, 0.9f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.015f, -0.09f);
    glVertex2f(-0.015f, -0.11f);
    glVertex2f(0.015f, -0.09f);
    glVertex2f(0.015f, -0.11f);
    glVertex2f(-0.015f, -0.10f);
    glVertex2f(0.015f, -0.10f);
    glEnd();
    glLineWidth(1.0f);

    // Railings
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-0.8f, -0.15f);
    glVertex2f(-0.7f, -0.15f);
    glVertex2f(-0.7f, -0.1f);
    glVertex2f(-0.8f, -0.1f);
    glVertex2f(0.7f, -0.15f);
    glVertex2f(0.8f, -0.15f);
    glVertex2f(0.8f, -0.1f);
    glVertex2f(0.7f, -0.1f);
    glEnd();

    // Thruster
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_LINES);
    glVertex2f(-0.75f, -0.15f);
    glVertex2f(-0.75f, -0.05f);
    glVertex2f(0.75f, -0.15f);
    glVertex2f(0.75f, -0.05f);
    glEnd();
}

void drawHelicopter() {

    glColor3f(0.3f, 0.3f, 0.4f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.07f, 0.02f);
    glVertex2f(0.03f, 0.02f);
    glVertex2f(0.05f, 0.00f);
    glVertex2f(0.03f, -0.02f);
    glVertex2f(-0.07f, -0.02f);
    glEnd();

    // Tail boom
    glColor3f(0.25f, 0.25f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(-0.07f, 0.01f);
    glVertex2f(-0.12f, 0.005f);
    glVertex2f(-0.12f, -0.005f);
    glVertex2f(-0.07f, -0.01f);
    glEnd();

    glColor3f(0.35f, 0.35f, 0.45f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.12f, 0.005f);
    glVertex2f(-0.13f, 0.025f);
    glVertex2f(-0.12f, -0.005f);
    glEnd();

    // Tail rotor
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_LINES);
    glVertex2f(-0.13f, 0.015f);
    glVertex2f(-0.13f, 0.035f);
    glEnd();

    // Main rotor base
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(-0.02f, 0.02f);
    glVertex2f(0.0f, 0.02f);
    glVertex2f(0.0f, 0.03f);
    glVertex2f(-0.02f, 0.03f);
    glEnd();

    // Main rotor blade
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.01f, 0.03f);
    glVertex2f(0.1f, 0.03f);
    glEnd();

    // Main rotor blade 2
    glBegin(GL_LINES);
    glVertex2f(-0.01f, 0.03f);
    glVertex2f(-0.01f, 0.13f);
    glEnd();
    glLineWidth(1.0f);

    // Skids
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.05f, -0.02f);
    glVertex2f(-0.05f, -0.04f);
    glVertex2f(0.0f, -0.04f);
    glVertex2f(0.0f, -0.02f);
    glEnd();

    //  Window
    glColor3f(0.6f, 0.7f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0.03f, 0.015f);
    glVertex2f(0.045f, 0.015f);
    glVertex2f(0.04f, -0.005f);
    glVertex2f(0.025f, -0.005f);
    glEnd();
}

//Display Animation

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawGradientSky();
    drawSunAndMoon();

    // Draw clouds
    glPushMatrix();
    glTranslatef(cloudOffset1, 0, 0);
    drawCloud(-0.8f, 0.8f);
    drawCloud(0.0f, 0.9f);
    if(cloudOffset1 > 2.2f) cloudOffset1 = -1.2f; // Reset cloud position
    glPopMatrix();

    glPushMatrix();
    glTranslatef(cloudOffset2, 0, 0);
    drawCloud(0.6f, 0.75f);
    drawCloud(-0.2f, 0.65f);
    if(cloudOffset2 > 2.2f) cloudOffset2 = -1.2f; // Reset cloud position
    glPopMatrix();

    drawWaves();

    drawHelicarrier();

    glPushMatrix();
    glTranslatef(-0.65f, -0.05f, 0.0f);
    glScalef(0.2592f, 0.2592f, 1.0f);
    drawHelicopter();
    glPopMatrix();

    // Helicopter 2
    glPushMatrix();
    glTranslatef(0.65f, -0.05f, 0.0f);
    glScalef(0.2592f, 0.2592f, 1.0f); // 20% bigger
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    drawHelicopter();
    glPopMatrix();



    // Standby Jet 1 lower-left
    glPushMatrix();
    glTranslatef(-0.45f, -0.2f, 0.0f); // beside runway
    glScalef(0.18375f, 0.18375f, 1.0f); // 20% bigger
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    drawFighterJet();
    glPopMatrix();

    // Standby Jet 2 central-right
    glPushMatrix();
    glTranslatef(0.3f, 0.0f, 0.0f); // Position on middle deck
    glScalef(0.18375f, 0.18375f, 1.0f); // 20% bigger
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
    drawFighterJet();
    glPopMatrix();


//jet appears on top of the carrier
    if (jetLandingState > 0 || jetPosX < 2.0f) {
        glPushMatrix();
        glTranslatef(jetPosX, jetPosY, 0.0f);
        glRotatef(jetAngle, 0.0f, 0.0f, 1.0f);
        glScalef(0.153125f, 0.153125f, 1.0f);
        drawFighterJet();
        glPopMatrix();
    }

    glFlush();
}

// The update function for animation
void update(int value) {
    waveOffset += 0.02f; // Animate waves
    cloudOffset1 += 0.001f; // Animate clouds
    cloudOffset2 += 0.002f;

    // Animate searchlight rotation
    searchlightAngle += 0.5f;
    if (searchlightAngle > 360.0f) {
        searchlightAngle -= 360.0f;
    }

    // Fighter jet landing animation logic
    switch (jetLandingState) {
        case 1:
            {
                landingCurrentTime += 1.0f; // Increment time
                if (landingCurrentTime > landingDuration) {
                    landingCurrentTime = landingDuration;
                }

                float t = landingCurrentTime / landingDuration;
                float oneMinusT = 1.0f - t;

                float control1X = -0.8f;
                float control1Y = 0.3f;

                float control2X = -0.4f;

                float control2Y = -0.10f;

                // Calculate current position on the curve
                float newPosX = oneMinusT * oneMinusT * oneMinusT * landingApproachStartPosX +
                                3 * oneMinusT * oneMinusT * t * control1X +
                                3 * oneMinusT * t * t * control2X +
                                t * t * t * landingTargetX;

                float newPosY = oneMinusT * oneMinusT * oneMinusT * landingApproachStartPosY +
                                3 * oneMinusT * oneMinusT * t * control1Y +
                                3 * oneMinusT * t * t * control2Y +
                                t * t * t * landingTargetY;

                // Calculate tangent for jet angle (derivative of Bézier curve)
                float tangentX = 3 * oneMinusT * oneMinusT * (control1X - landingApproachStartPosX) +
                                 6 * oneMinusT * t * (control2X - control1X) +
                                 3 * t * t * (landingTargetX - control2X);

                float tangentY = 3 * oneMinusT * oneMinusT * (control1Y - landingApproachStartPosY) +
                                 6 * oneMinusT * t * (control2Y - control1Y) +
                                 3 * t * t * (landingTargetY - control2Y);

                jetPosX = newPosX;
                jetPosY = newPosY;
                jetAngle = atan2(tangentY, tangentX) * 180.0f / PI;

                // animation is complete
                if (landingCurrentTime >= landingDuration) {
                    jetLandingState = 2;
                    jetPosX = landingTargetX;
                    jetPosY = landingTargetY;
                    jetAngle = 0.0f;
                    jetSpeed = 0.015f;
                }
            }
            break;

        case 2: // State 2: Rolling on the deck
            {
                jetPosX += jetSpeed; // right along the runway
                jetSpeed *= 0.985f;  // Decelerate the jet

                // Stop the jet once it reaches the desired parked X position
                if (jetPosX >= jetParkedX) {
                    jetLandingState = 0;
                    jetSpeed = 0.0f;
                    jetPosX = jetParkedX;
                }
            }
            break;
    }

    // Day/Night Cycle Logic
    if(isNightTransition) {
        if(skyGreen > 0.2f) skyGreen -= 0.001f;
        if(skyRed > 0.05f) skyRed -= 0.001f;
        if(skyBlue > 0.15f) skyBlue -= 0.002f;
        if(sunAlpha > 0.0f) sunAlpha -= 0.005f;
        if(starAlpha < 1.0f) starAlpha += 0.005f;
        if(lightGlow < 1.0f) lightGlow += 0.005f;
    } else {
        if(skyGreen < 0.7f) skyGreen += 0.001f;
        if(skyRed < 0.4f) skyRed += 0.001f;
        if(skyBlue < 1.0f) skyBlue += 0.002f;
        if(sunAlpha < 1.0f) sunAlpha += 0.005f;
        if(starAlpha > 0.0f) starAlpha -= 0.005f;
        if(lightGlow > 0.0f) lightGlow -= 0.005f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// user input
void keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 'n':
        case 'N':
            isNightTransition = true; // night transition
            break;
        case 'd':
        case 'D':
            isNightTransition = false; // Initiate day transition
            break;
        case 'l':
        case 'L':
            if (jetLandingState == 0) {
                jetLandingState = 1;
                landingApproachStartPosX = -1.2f;
                landingApproachStartPosY = 0.6f;
                jetPosX = landingApproachStartPosX;
                jetPosY = landingApproachStartPosY;
                jetAngle = 0.0f;
                landingCurrentTime = 0.0f;
            }
            break;
    }
}

// - Main ---
int main(int argc, char** argv) {
    glutInit(&argc, argv); // Initialize GLUT
    glutInitWindowSize(800, 600); // Set window size
    glutInitWindowPosition(300, 200); // Set window position
    glutCreateWindow("Helicarrier Scene - 'L' to Land, 'N' for Night, 'D' for Day");

    glEnable(GL_POINT_SMOOTH); // Enable anti-aliasing for points
    glEnable(GL_BLEND); // Enable blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending function

    glutDisplayFunc(display); // Register the display callback function
    glutKeyboardFunc(keyboard); // Register the keyboard callback function
    glutTimerFunc(16, update, 0); // Start the animation timer
    sndPlaySound("ocean", SND_ASYNC);
    glutMainLoop(); // Enter the GLUT event processing loop
    return 0;
}
