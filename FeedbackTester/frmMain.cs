//-----------------------------------------------------------------------------
// File: frmMain.cs
//
// Desc: The Feedback sample shows Force Feedback via devices that support it.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;

namespace Feedback
{
    public class frmMain : System.Windows.Forms.Form
    {
        /// <summary>
        /// This structure will contain information about an effect,
        /// its Effect structure, and the DirectInputEffect object.
        /// </summary>
        private struct EffectDescription
        {           
            public EffectInformation info;
            public EffectObject effectSelected;
            public override string ToString() 
            {
                return info.Name;
            }
        }
        
        private Device applicationDevice; //DirectInput device object.
        private EffectObject effectSelected; //The currently selected effect.
        private int[] axis; //Holds the FF axes offsets.
        private bool isChanging; // Flag that is set when that app is changing control values.

        #region Window control declarations
        internal System.Windows.Forms.Label Label1;
        internal System.Windows.Forms.GroupBox gbGeneralParams;
        internal System.Windows.Forms.TrackBar GeneralPeriod;
        internal System.Windows.Forms.Label GeneralPeriodLabel;
        internal System.Windows.Forms.TrackBar GeneralGain;
        internal System.Windows.Forms.Label GeneralGainLabel;
        internal System.Windows.Forms.TrackBar GeneralDuration;
        internal System.Windows.Forms.Label GeneralDurationLabel;
        internal System.Windows.Forms.ListBox lstEffects;
        internal System.Windows.Forms.GroupBox gbTypeContainer;
        internal System.Windows.Forms.GroupBox GroupPeriodForce;
        internal System.Windows.Forms.Label PeriodicPeriodLabel;
        internal System.Windows.Forms.TrackBar PeriodicPeriod;
        internal System.Windows.Forms.Label PeriodicPhaseLabel;
        internal System.Windows.Forms.TrackBar PeriodicPhase;
        internal System.Windows.Forms.Label PeriodicOffsetLabel;
        internal System.Windows.Forms.TrackBar PeriodicOffset;
        internal System.Windows.Forms.Label PeriodicMagnitudeLabel;
        internal System.Windows.Forms.TrackBar PeriodicMagnitude;
        internal System.Windows.Forms.GroupBox GroupConstantForce;
        internal System.Windows.Forms.Label Magnitude;
        internal System.Windows.Forms.TrackBar ConstantForceMagnitude;
        internal System.Windows.Forms.GroupBox GroupConditionalForce;
        internal System.Windows.Forms.RadioButton rbConditionalAxis2;
        internal System.Windows.Forms.RadioButton ConditionalAxis1;
        internal System.Windows.Forms.Label ConditionalPositiveSaturationLabel;
        internal System.Windows.Forms.TrackBar ConditionalPositiveSaturation;
        internal System.Windows.Forms.Label ConditionalNegativeSaturationLabel;
        internal System.Windows.Forms.TrackBar ConditionalNegativeSaturation;
        internal System.Windows.Forms.Label ConditionalPositiveCoefficientLabel;
        internal System.Windows.Forms.TrackBar ConditionalPositiveCoefficient;
        internal System.Windows.Forms.Label ConditionalNegativeCoeffcientLabel;
        internal System.Windows.Forms.TrackBar ConditionalNegativeCoeffcient;
        internal System.Windows.Forms.Label ConditionalOffsetLabel;
        internal System.Windows.Forms.TrackBar ConditionalOffset;
        internal System.Windows.Forms.Label ConditionalDeadBandLabel;
        internal System.Windows.Forms.TrackBar ConditionalDeadBand;
        internal System.Windows.Forms.GroupBox GroupRampForce;
        internal System.Windows.Forms.Label RangeEndLabel;
        internal System.Windows.Forms.TrackBar RangeEnd;
        internal System.Windows.Forms.Label RangeStartLabel;
        internal System.Windows.Forms.TrackBar RangeStart;
        internal System.Windows.Forms.GroupBox EnvelopeGroupBox;
        internal System.Windows.Forms.TrackBar EnvelopeFadeTime;
        internal System.Windows.Forms.Label EnvelopeFadeTimeLabel;
        internal System.Windows.Forms.TrackBar EnvelopeFadeLevel;
        internal System.Windows.Forms.Label EnvelopeFadeLevelLabel;
        internal System.Windows.Forms.TrackBar EnvelopeAttackTime;
        internal System.Windows.Forms.Label EnvelopeAttackTimeLabel;
        internal System.Windows.Forms.TrackBar EnvelopeAttackLevel;
        internal System.Windows.Forms.Label EnvelopeAttackLevelLabel;
        internal System.Windows.Forms.CheckBox chkUseEnvelope;
        internal System.Windows.Forms.GroupBox DirectionGroupBox;
        internal System.Windows.Forms.RadioButton NorthEast;
        internal System.Windows.Forms.RadioButton East;
        internal System.Windows.Forms.RadioButton SouthEast;
        internal System.Windows.Forms.RadioButton South;
        internal System.Windows.Forms.RadioButton SouthWest;
        internal System.Windows.Forms.RadioButton West;
        internal System.Windows.Forms.RadioButton NorthWest;
        internal System.Windows.Forms.RadioButton North;
        #endregion
        private System.ComponentModel.Container components = null;


        
        
        public frmMain()
        {
            try
            {
                // Load the icon from our resources
                System.Resources.ResourceManager resources = new System.Resources.ResourceManager(this.GetType());
                this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            }
            catch
            {
                // It's no big deal if we can't load our icons, but try to load the embedded one
                try { this.Icon = new System.Drawing.Icon(this.GetType(), "directx.ico"); } 
                catch {}
            }
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();            
        }

        
        
        
        /// <summary>
        /// Initializes DirectInput.
        /// </summary>
        private bool InitializeDirectInput()
        {
            try
            {
                //Enumerate all joysticks that are attached to the system and have FF capabilities
                foreach (DeviceInstance instanceDevice in Manager.GetDevices(DeviceClass.GameControl, EnumDevicesFlags.ForceFeeback | EnumDevicesFlags.AttachedOnly))
                {
                    applicationDevice = new Device(instanceDevice.InstanceGuid);
                    foreach (DeviceObjectInstance instanceObject in applicationDevice.GetObjects(DeviceObjectTypeFlags.Axis))  // Get info about all the FF axis on the device
                    {
                        int[] temp;

                        if ((instanceObject.Flags & (int)ObjectInstanceFlags.Actuator) != 0)
                        {
                            if (null != axis)
                            {
                                temp = new int[axis.Length + 1];                   
                                axis.CopyTo(temp,0);
                                axis = temp;
                            }
                            else
                            {
                                axis = new int[1];
                            }
                            // Store the offset of each axis.
                            axis[axis.Length - 1] = instanceObject.Offset;
                            // Don't need to enumerate any more if 2 were found.
                            if (2 == axis.Length)
                                break;
                        }
                    }

                    if (null == applicationDevice)
                    {
                        MessageBox.Show("No force feedback devices found attached to the system. Sample will now exit.", "No suitable device", 
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return false;
                    }

                    if (axis.Length - 1 >= 1)
                        // Grab any device that contains at least one axis.
                        break;
                    else
                    {
                        axis = null;
                        applicationDevice.Dispose();
                        applicationDevice = null;
                    }
                }

                //Turn off autocenter
                applicationDevice.Properties.AutoCenter = false;

                //Set the format of the device to that of a joystick
                applicationDevice.SetDataFormat(DeviceDataFormat.Joystick);

                //Enumerate all the effects on the device
                foreach (EffectInformation ei in applicationDevice.GetEffects(EffectType.All))
                {
                    // Handles the enumeration of effects.

                    EffectObject effectSelected; 
                    EffectDescription description = new EffectDescription(); 
                    Effect eff;

                    if (DInputHelper.GetTypeCode(ei.EffectType) == (int)EffectType.CustomForce)
                    {
                        // Can't create a custom force without info from the hardware vendor, so skip this effect.
                        continue;
                    }
                    else if (DInputHelper.GetTypeCode(ei.EffectType) == (int)EffectType.Periodic)
                    {
                        // This is to filter out any Periodic effects. There are known
                        // issues with Periodic effects that will be addressed post-developer preview.
                        continue;
                    }
                    else if (DInputHelper.GetTypeCode(ei.EffectType) == (int)EffectType.Hardware)
                    {
                        if ((ei.StaticParams & (int)EffectParameterFlags.TypeSpecificParams) != 0)
                            // Can't create a hardware force without info from the hardware vendor.
                            continue;
                    }

                    // Fill in some generic values for the effect.
                    eff = FillEffStruct((EffectType)ei.EffectType);

                    // Create the effect, using the passed in guid.
                    effectSelected = new EffectObject(ei.EffectGuid, eff, applicationDevice);

                    // Fill in the EffectDescription structure.
                    description.effectSelected = effectSelected;
                    description.info = ei;

                    // Add this effect to the listbox, displaying the name of the effect.
                    lstEffects.Items.Add(description);
                }

                if (0 == lstEffects.Items.Count)
                {
                    // If this device has no downloadable effects, end the app.
                    MessageBox.Show("This device does not contain any downloadable effects, app will exit.");
                    // The app will validate all DirectInput objects in the frmMain_Load() event.
                    // When one is found missing, this will cause the app to exit.
                }

                // Set the cooperative level of the device as an exclusive
                // foreground device, and attach it to the form's window handle.
                applicationDevice.SetCooperativeLevel(this, CooperativeLevelFlags.Foreground | CooperativeLevelFlags.Exclusive);

                // Make the first index of the listbox selected
                lstEffects.SelectedIndex = 0;
                return true;
            }
            catch
            {
                MessageBox.Show("No force feedback devices found attached to the system. Sample will now exit.", "No suitable device", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
        }
        
        
        
        
        /// <summary>
        /// Fills in generic values in an effect struct.
        /// </summary>
        private Effect FillEffStruct(EffectType eif)
        {
            Effect eff = new Effect();

            // Allocate some memory for directions and axis.
            eff.SetDirection(new int[axis.Length]);
            eff.SetAxes(new int[axis.Length]);

            eff.EffectType = eif;           
            eff.ConditionStruct =  new Condition[axis.Length];
            eff.Duration = (int)DI.Infinite;
            eff.Gain = 10000;
            eff.SamplePeriod = 0;
            eff.TriggerButton = (int)Microsoft.DirectX.DirectInput.Button.NoTrigger;
            eff.TriggerRepeatInterval = (int)DI.Infinite;
            eff.Flags = EffectFlags.ObjectOffsets | EffectFlags.Cartesian;
            eff.SetAxes(axis);

            return eff;
        }
        
        
        
        
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null) 
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        
        
        
        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.Label1 = new System.Windows.Forms.Label();
            this.gbGeneralParams = new System.Windows.Forms.GroupBox();
            this.GeneralPeriod = new System.Windows.Forms.TrackBar();
            this.GeneralPeriodLabel = new System.Windows.Forms.Label();
            this.GeneralGain = new System.Windows.Forms.TrackBar();
            this.GeneralGainLabel = new System.Windows.Forms.Label();
            this.GeneralDuration = new System.Windows.Forms.TrackBar();
            this.GeneralDurationLabel = new System.Windows.Forms.Label();
            this.lstEffects = new System.Windows.Forms.ListBox();
            this.gbTypeContainer = new System.Windows.Forms.GroupBox();
            this.GroupConditionalForce = new System.Windows.Forms.GroupBox();
            this.rbConditionalAxis2 = new System.Windows.Forms.RadioButton();
            this.ConditionalAxis1 = new System.Windows.Forms.RadioButton();
            this.ConditionalPositiveSaturationLabel = new System.Windows.Forms.Label();
            this.ConditionalPositiveSaturation = new System.Windows.Forms.TrackBar();
            this.ConditionalNegativeSaturationLabel = new System.Windows.Forms.Label();
            this.ConditionalNegativeSaturation = new System.Windows.Forms.TrackBar();
            this.ConditionalPositiveCoefficientLabel = new System.Windows.Forms.Label();
            this.ConditionalPositiveCoefficient = new System.Windows.Forms.TrackBar();
            this.ConditionalNegativeCoeffcientLabel = new System.Windows.Forms.Label();
            this.ConditionalNegativeCoeffcient = new System.Windows.Forms.TrackBar();
            this.ConditionalOffsetLabel = new System.Windows.Forms.Label();
            this.ConditionalOffset = new System.Windows.Forms.TrackBar();
            this.ConditionalDeadBandLabel = new System.Windows.Forms.Label();
            this.ConditionalDeadBand = new System.Windows.Forms.TrackBar();
            this.GroupPeriodForce = new System.Windows.Forms.GroupBox();
            this.PeriodicPeriodLabel = new System.Windows.Forms.Label();
            this.PeriodicPeriod = new System.Windows.Forms.TrackBar();
            this.PeriodicPhaseLabel = new System.Windows.Forms.Label();
            this.PeriodicPhase = new System.Windows.Forms.TrackBar();
            this.PeriodicOffsetLabel = new System.Windows.Forms.Label();
            this.PeriodicOffset = new System.Windows.Forms.TrackBar();
            this.PeriodicMagnitudeLabel = new System.Windows.Forms.Label();
            this.PeriodicMagnitude = new System.Windows.Forms.TrackBar();
            this.GroupRampForce = new System.Windows.Forms.GroupBox();
            this.RangeEndLabel = new System.Windows.Forms.Label();
            this.RangeEnd = new System.Windows.Forms.TrackBar();
            this.RangeStartLabel = new System.Windows.Forms.Label();
            this.RangeStart = new System.Windows.Forms.TrackBar();
            this.GroupConstantForce = new System.Windows.Forms.GroupBox();
            this.Magnitude = new System.Windows.Forms.Label();
            this.ConstantForceMagnitude = new System.Windows.Forms.TrackBar();
            this.EnvelopeGroupBox = new System.Windows.Forms.GroupBox();
            this.EnvelopeFadeTime = new System.Windows.Forms.TrackBar();
            this.EnvelopeFadeTimeLabel = new System.Windows.Forms.Label();
            this.EnvelopeFadeLevel = new System.Windows.Forms.TrackBar();
            this.EnvelopeFadeLevelLabel = new System.Windows.Forms.Label();
            this.EnvelopeAttackTime = new System.Windows.Forms.TrackBar();
            this.EnvelopeAttackTimeLabel = new System.Windows.Forms.Label();
            this.EnvelopeAttackLevel = new System.Windows.Forms.TrackBar();
            this.EnvelopeAttackLevelLabel = new System.Windows.Forms.Label();
            this.chkUseEnvelope = new System.Windows.Forms.CheckBox();
            this.DirectionGroupBox = new System.Windows.Forms.GroupBox();
            this.NorthEast = new System.Windows.Forms.RadioButton();
            this.East = new System.Windows.Forms.RadioButton();
            this.SouthEast = new System.Windows.Forms.RadioButton();
            this.South = new System.Windows.Forms.RadioButton();
            this.SouthWest = new System.Windows.Forms.RadioButton();
            this.West = new System.Windows.Forms.RadioButton();
            this.NorthWest = new System.Windows.Forms.RadioButton();
            this.North = new System.Windows.Forms.RadioButton();
            this.gbGeneralParams.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GeneralPeriod)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.GeneralGain)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.GeneralDuration)).BeginInit();
            this.gbTypeContainer.SuspendLayout();
            this.GroupConditionalForce.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalPositiveSaturation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalNegativeSaturation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalPositiveCoefficient)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalNegativeCoeffcient)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalOffset)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalDeadBand)).BeginInit();
            this.GroupPeriodForce.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PeriodicPeriod)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PeriodicPhase)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PeriodicOffset)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PeriodicMagnitude)).BeginInit();
            this.GroupRampForce.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.RangeEnd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RangeStart)).BeginInit();
            this.GroupConstantForce.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ConstantForceMagnitude)).BeginInit();
            this.EnvelopeGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.EnvelopeFadeTime)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EnvelopeFadeLevel)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EnvelopeAttackTime)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EnvelopeAttackLevel)).BeginInit();
            this.DirectionGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // Label1
            // 
            this.Label1.AutoSize = true;
            this.Label1.Location = new System.Drawing.Point(6, 6);
            this.Label1.Name = "Label1";
            this.Label1.Size = new System.Drawing.Size(91, 13);
            this.Label1.TabIndex = 7;
            this.Label1.Text = "Available Effects:";
            // 
            // gbGeneralParams
            // 
            this.gbGeneralParams.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                          this.GeneralPeriod,
                                                                                          this.GeneralPeriodLabel,
                                                                                          this.GeneralGain,
                                                                                          this.GeneralGainLabel,
                                                                                          this.GeneralDuration,
                                                                                          this.GeneralDurationLabel});
            this.gbGeneralParams.Location = new System.Drawing.Point(6, 262);
            this.gbGeneralParams.Name = "gbGeneralParams";
            this.gbGeneralParams.Size = new System.Drawing.Size(224, 216);
            this.gbGeneralParams.TabIndex = 11;
            this.gbGeneralParams.TabStop = false;
            this.gbGeneralParams.Text = "General Parameters";
            // 
            // GeneralPeriod
            // 
            this.GeneralPeriod.AutoSize = false;
            this.GeneralPeriod.Location = new System.Drawing.Point(8, 152);
            this.GeneralPeriod.Maximum = 100000;
            this.GeneralPeriod.Name = "GeneralPeriod";
            this.GeneralPeriod.Size = new System.Drawing.Size(208, 45);
            this.GeneralPeriod.TabIndex = 5;
            this.GeneralPeriod.TickFrequency = 5000;
            this.GeneralPeriod.Scroll += new System.EventHandler(this.GenScroll);
            // 
            // GeneralPeriodLabel
            // 
            this.GeneralPeriodLabel.Location = new System.Drawing.Point(16, 136);
            this.GeneralPeriodLabel.Name = "GeneralPeriodLabel";
            this.GeneralPeriodLabel.Size = new System.Drawing.Size(192, 16);
            this.GeneralPeriodLabel.TabIndex = 4;
            this.GeneralPeriodLabel.Text = "Sample Period: Default";
            // 
            // GeneralGain
            // 
            this.GeneralGain.AutoSize = false;
            this.GeneralGain.Location = new System.Drawing.Point(8, 96);
            this.GeneralGain.Maximum = 10000;
            this.GeneralGain.Name = "GeneralGain";
            this.GeneralGain.Size = new System.Drawing.Size(208, 45);
            this.GeneralGain.TabIndex = 3;
            this.GeneralGain.TickFrequency = 1000;
            this.GeneralGain.Value = 10000;
            this.GeneralGain.Scroll += new System.EventHandler(this.GenScroll);
            // 
            // GeneralGainLabel
            // 
            this.GeneralGainLabel.Location = new System.Drawing.Point(16, 84);
            this.GeneralGainLabel.Name = "GeneralGainLabel";
            this.GeneralGainLabel.Size = new System.Drawing.Size(192, 16);
            this.GeneralGainLabel.TabIndex = 2;
            this.GeneralGainLabel.Text = "Effect Gain: 10000";
            // 
            // GeneralDuration
            // 
            this.GeneralDuration.AutoSize = false;
            this.GeneralDuration.LargeChange = 2;
            this.GeneralDuration.Location = new System.Drawing.Point(8, 48);
            this.GeneralDuration.Minimum = 1;
            this.GeneralDuration.Name = "GeneralDuration";
            this.GeneralDuration.Size = new System.Drawing.Size(208, 45);
            this.GeneralDuration.TabIndex = 1;
            this.GeneralDuration.Value = 10;
            this.GeneralDuration.Scroll += new System.EventHandler(this.GenScroll);
            // 
            // GeneralDurationLabel
            // 
            this.GeneralDurationLabel.Location = new System.Drawing.Point(16, 32);
            this.GeneralDurationLabel.Name = "GeneralDurationLabel";
            this.GeneralDurationLabel.Size = new System.Drawing.Size(192, 16);
            this.GeneralDurationLabel.TabIndex = 0;
            this.GeneralDurationLabel.Text = "Effect Duration: Infinite";
            // 
            // lstEffects
            // 
            this.lstEffects.Location = new System.Drawing.Point(6, 22);
            this.lstEffects.Name = "lstEffects";
            this.lstEffects.Size = new System.Drawing.Size(225, 225);
            this.lstEffects.TabIndex = 6;
            this.lstEffects.SelectedIndexChanged += new System.EventHandler(this.lstEffects_SelectedIndexChanged);
            // 
            // gbTypeContainer
            // 
            this.gbTypeContainer.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                          this.GroupConditionalForce,
                                                                                          this.GroupPeriodForce,
                                                                                          this.GroupRampForce,
                                                                                          this.GroupConstantForce});
            this.gbTypeContainer.Location = new System.Drawing.Point(246, 14);
            this.gbTypeContainer.Name = "gbTypeContainer";
            this.gbTypeContainer.Size = new System.Drawing.Size(361, 233);
            this.gbTypeContainer.TabIndex = 8;
            this.gbTypeContainer.TabStop = false;
            this.gbTypeContainer.Text = "Type-Specific Parameters";
            // 
            // GroupConditionalForce
            // 
            this.GroupConditionalForce.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                                this.rbConditionalAxis2,
                                                                                                this.ConditionalAxis1,
                                                                                                this.ConditionalPositiveSaturationLabel,
                                                                                                this.ConditionalPositiveSaturation,
                                                                                                this.ConditionalNegativeSaturationLabel,
                                                                                                this.ConditionalNegativeSaturation,
                                                                                                this.ConditionalPositiveCoefficientLabel,
                                                                                                this.ConditionalPositiveCoefficient,
                                                                                                this.ConditionalNegativeCoeffcientLabel,
                                                                                                this.ConditionalNegativeCoeffcient,
                                                                                                this.ConditionalOffsetLabel,
                                                                                                this.ConditionalOffset,
                                                                                                this.ConditionalDeadBandLabel,
                                                                                                this.ConditionalDeadBand});
            this.GroupConditionalForce.Location = new System.Drawing.Point(8, 16);
            this.GroupConditionalForce.Name = "GroupConditionalForce";
            this.GroupConditionalForce.Size = new System.Drawing.Size(344, 200);
            this.GroupConditionalForce.TabIndex = 3;
            this.GroupConditionalForce.TabStop = false;
            this.GroupConditionalForce.Tag = "";
            this.GroupConditionalForce.Text = "Conditional Force";
            this.GroupConditionalForce.Visible = false;
            // 
            // rbConditionalAxis2
            // 
            this.rbConditionalAxis2.Location = new System.Drawing.Point(160, 176);
            this.rbConditionalAxis2.Name = "rbConditionalAxis2";
            this.rbConditionalAxis2.Size = new System.Drawing.Size(56, 16);
            this.rbConditionalAxis2.TabIndex = 13;
            this.rbConditionalAxis2.Text = "Axis 2";
            this.rbConditionalAxis2.CheckedChanged += new System.EventHandler(this.ConditionalAxisChanged);
            // 
            // ConditionalAxis1
            // 
            this.ConditionalAxis1.Checked = true;
            this.ConditionalAxis1.Location = new System.Drawing.Point(160, 160);
            this.ConditionalAxis1.Name = "ConditionalAxis1";
            this.ConditionalAxis1.Size = new System.Drawing.Size(56, 16);
            this.ConditionalAxis1.TabIndex = 12;
            this.ConditionalAxis1.TabStop = true;
            this.ConditionalAxis1.Text = "Axis 1";
            this.ConditionalAxis1.CheckedChanged += new System.EventHandler(this.ConditionalAxisChanged);
            // 
            // ConditionalPositiveSaturationLabel
            // 
            this.ConditionalPositiveSaturationLabel.AutoSize = true;
            this.ConditionalPositiveSaturationLabel.Location = new System.Drawing.Point(192, 120);
            this.ConditionalPositiveSaturationLabel.Name = "ConditionalPositiveSaturationLabel";
            this.ConditionalPositiveSaturationLabel.Size = new System.Drawing.Size(136, 13);
            this.ConditionalPositiveSaturationLabel.TabIndex = 11;
            this.ConditionalPositiveSaturationLabel.Text = "Positive Saturation: 10000";
            // 
            // ConditionalPositiveSaturation
            // 
            this.ConditionalPositiveSaturation.AutoSize = false;
            this.ConditionalPositiveSaturation.LargeChange = 1000;
            this.ConditionalPositiveSaturation.Location = new System.Drawing.Point(192, 136);
            this.ConditionalPositiveSaturation.Maximum = 10000;
            this.ConditionalPositiveSaturation.Name = "ConditionalPositiveSaturation";
            this.ConditionalPositiveSaturation.Size = new System.Drawing.Size(136, 45);
            this.ConditionalPositiveSaturation.SmallChange = 100;
            this.ConditionalPositiveSaturation.TabIndex = 10;
            this.ConditionalPositiveSaturation.TickFrequency = 1000;
            this.ConditionalPositiveSaturation.Value = 10000;
            this.ConditionalPositiveSaturation.Scroll += new System.EventHandler(this.ConditionalScroll);
            // 
            // ConditionalNegativeSaturationLabel
            // 
            this.ConditionalNegativeSaturationLabel.AutoSize = true;
            this.ConditionalNegativeSaturationLabel.Location = new System.Drawing.Point(24, 120);
            this.ConditionalNegativeSaturationLabel.Name = "ConditionalNegativeSaturationLabel";
            this.ConditionalNegativeSaturationLabel.Size = new System.Drawing.Size(141, 13);
            this.ConditionalNegativeSaturationLabel.TabIndex = 9;
            this.ConditionalNegativeSaturationLabel.Text = "Negative Saturation: 10000";
            // 
            // ConditionalNegativeSaturation
            // 
            this.ConditionalNegativeSaturation.AutoSize = false;
            this.ConditionalNegativeSaturation.LargeChange = 1000;
            this.ConditionalNegativeSaturation.Location = new System.Drawing.Point(24, 136);
            this.ConditionalNegativeSaturation.Maximum = 10000;
            this.ConditionalNegativeSaturation.Name = "ConditionalNegativeSaturation";
            this.ConditionalNegativeSaturation.Size = new System.Drawing.Size(136, 45);
            this.ConditionalNegativeSaturation.SmallChange = 100;
            this.ConditionalNegativeSaturation.TabIndex = 8;
            this.ConditionalNegativeSaturation.TickFrequency = 1000;
            this.ConditionalNegativeSaturation.Value = 10000;
            this.ConditionalNegativeSaturation.Scroll += new System.EventHandler(this.ConditionalScroll);
            // 
            // ConditionalPositiveCoefficientLabel
            // 
            this.ConditionalPositiveCoefficientLabel.AutoSize = true;
            this.ConditionalPositiveCoefficientLabel.Location = new System.Drawing.Point(192, 72);
            this.ConditionalPositiveCoefficientLabel.Name = "ConditionalPositiveCoefficientLabel";
            this.ConditionalPositiveCoefficientLabel.Size = new System.Drawing.Size(113, 13);
            this.ConditionalPositiveCoefficientLabel.TabIndex = 7;
            this.ConditionalPositiveCoefficientLabel.Text = "Positive Coefficient: 0";
            // 
            // ConditionalPositiveCoefficient
            // 
            this.ConditionalPositiveCoefficient.AutoSize = false;
            this.ConditionalPositiveCoefficient.LargeChange = 1000;
            this.ConditionalPositiveCoefficient.Location = new System.Drawing.Point(192, 88);
            this.ConditionalPositiveCoefficient.Maximum = 10000;
            this.ConditionalPositiveCoefficient.Minimum = -10000;
            this.ConditionalPositiveCoefficient.Name = "ConditionalPositiveCoefficient";
            this.ConditionalPositiveCoefficient.Size = new System.Drawing.Size(136, 45);
            this.ConditionalPositiveCoefficient.SmallChange = 100;
            this.ConditionalPositiveCoefficient.TabIndex = 6;
            this.ConditionalPositiveCoefficient.TickFrequency = 1000;
            this.ConditionalPositiveCoefficient.Scroll += new System.EventHandler(this.ConditionalScroll);
            // 
            // ConditionalNegativeCoeffcientLabel
            // 
            this.ConditionalNegativeCoeffcientLabel.AutoSize = true;
            this.ConditionalNegativeCoeffcientLabel.Location = new System.Drawing.Point(24, 72);
            this.ConditionalNegativeCoeffcientLabel.Name = "ConditionalNegativeCoeffcientLabel";
            this.ConditionalNegativeCoeffcientLabel.Size = new System.Drawing.Size(118, 13);
            this.ConditionalNegativeCoeffcientLabel.TabIndex = 5;
            this.ConditionalNegativeCoeffcientLabel.Text = "Negative Coefficient: 0";
            // 
            // ConditionalNegativeCoeffcient
            // 
            this.ConditionalNegativeCoeffcient.AutoSize = false;
            this.ConditionalNegativeCoeffcient.LargeChange = 1000;
            this.ConditionalNegativeCoeffcient.Location = new System.Drawing.Point(24, 88);
            this.ConditionalNegativeCoeffcient.Maximum = 10000;
            this.ConditionalNegativeCoeffcient.Minimum = -10000;
            this.ConditionalNegativeCoeffcient.Name = "ConditionalNegativeCoeffcient";
            this.ConditionalNegativeCoeffcient.Size = new System.Drawing.Size(136, 45);
            this.ConditionalNegativeCoeffcient.SmallChange = 100;
            this.ConditionalNegativeCoeffcient.TabIndex = 4;
            this.ConditionalNegativeCoeffcient.TickFrequency = 1000;
            this.ConditionalNegativeCoeffcient.Scroll += new System.EventHandler(this.ConditionalScroll);
            // 
            // ConditionalOffsetLabel
            // 
            this.ConditionalOffsetLabel.AutoSize = true;
            this.ConditionalOffsetLabel.Location = new System.Drawing.Point(192, 24);
            this.ConditionalOffsetLabel.Name = "ConditionalOffsetLabel";
            this.ConditionalOffsetLabel.Size = new System.Drawing.Size(47, 13);
            this.ConditionalOffsetLabel.TabIndex = 3;
            this.ConditionalOffsetLabel.Text = "Offset: 0";
            // 
            // ConditionalOffset
            // 
            this.ConditionalOffset.AutoSize = false;
            this.ConditionalOffset.LargeChange = 1000;
            this.ConditionalOffset.Location = new System.Drawing.Point(192, 40);
            this.ConditionalOffset.Maximum = 10000;
            this.ConditionalOffset.Minimum = -10000;
            this.ConditionalOffset.Name = "ConditionalOffset";
            this.ConditionalOffset.Size = new System.Drawing.Size(136, 45);
            this.ConditionalOffset.SmallChange = 100;
            this.ConditionalOffset.TabIndex = 2;
            this.ConditionalOffset.TickFrequency = 1000;
            this.ConditionalOffset.Scroll += new System.EventHandler(this.ConditionalScroll);
            // 
            // ConditionalDeadBandLabel
            // 
            this.ConditionalDeadBandLabel.AutoSize = true;
            this.ConditionalDeadBandLabel.Location = new System.Drawing.Point(24, 24);
            this.ConditionalDeadBandLabel.Name = "ConditionalDeadBandLabel";
            this.ConditionalDeadBandLabel.Size = new System.Drawing.Size(73, 13);
            this.ConditionalDeadBandLabel.TabIndex = 1;
            this.ConditionalDeadBandLabel.Text = "Dead Band: 0";
            // 
            // ConditionalDeadBand
            // 
            this.ConditionalDeadBand.AutoSize = false;
            this.ConditionalDeadBand.LargeChange = 1000;
            this.ConditionalDeadBand.Location = new System.Drawing.Point(24, 40);
            this.ConditionalDeadBand.Maximum = 10000;
            this.ConditionalDeadBand.Name = "ConditionalDeadBand";
            this.ConditionalDeadBand.Size = new System.Drawing.Size(136, 45);
            this.ConditionalDeadBand.SmallChange = 100;
            this.ConditionalDeadBand.TabIndex = 0;
            this.ConditionalDeadBand.TickFrequency = 1000;
            this.ConditionalDeadBand.Scroll += new System.EventHandler(this.ConditionalScroll);
            // 
            // GroupPeriodForce
            // 
            this.GroupPeriodForce.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                           this.PeriodicPeriodLabel,
                                                                                           this.PeriodicPeriod,
                                                                                           this.PeriodicPhaseLabel,
                                                                                           this.PeriodicPhase,
                                                                                           this.PeriodicOffsetLabel,
                                                                                           this.PeriodicOffset,
                                                                                           this.PeriodicMagnitudeLabel,
                                                                                           this.PeriodicMagnitude});
            this.GroupPeriodForce.Location = new System.Drawing.Point(8, 16);
            this.GroupPeriodForce.Name = "GroupPeriodForce";
            this.GroupPeriodForce.Size = new System.Drawing.Size(344, 200);
            this.GroupPeriodForce.TabIndex = 2;
            this.GroupPeriodForce.TabStop = false;
            this.GroupPeriodForce.Tag = "";
            this.GroupPeriodForce.Text = "Periodic Force";
            this.GroupPeriodForce.Visible = false;
            // 
            // PeriodicPeriodLabel
            // 
            this.PeriodicPeriodLabel.AutoSize = true;
            this.PeriodicPeriodLabel.Location = new System.Drawing.Point(96, 142);
            this.PeriodicPeriodLabel.Name = "PeriodicPeriodLabel";
            this.PeriodicPeriodLabel.Size = new System.Drawing.Size(49, 13);
            this.PeriodicPeriodLabel.TabIndex = 7;
            this.PeriodicPeriodLabel.Text = "Period: 0";
            // 
            // PeriodicPeriod
            // 
            this.PeriodicPeriod.LargeChange = 1000;
            this.PeriodicPeriod.Location = new System.Drawing.Point(24, 153);
            this.PeriodicPeriod.Maximum = 500000;
            this.PeriodicPeriod.Name = "PeriodicPeriod";
            this.PeriodicPeriod.Size = new System.Drawing.Size(304, 45);
            this.PeriodicPeriod.SmallChange = 100;
            this.PeriodicPeriod.TabIndex = 6;
            this.PeriodicPeriod.TickFrequency = 20000;
            this.PeriodicPeriod.Scroll += new System.EventHandler(this.PeriodicScroll);
            // 
            // PeriodicPhaseLabel
            // 
            this.PeriodicPhaseLabel.AutoSize = true;
            this.PeriodicPhaseLabel.Location = new System.Drawing.Point(96, 100);
            this.PeriodicPhaseLabel.Name = "PeriodicPhaseLabel";
            this.PeriodicPhaseLabel.Size = new System.Drawing.Size(49, 13);
            this.PeriodicPhaseLabel.TabIndex = 5;
            this.PeriodicPhaseLabel.Text = "Phase: 0";
            // 
            // PeriodicPhase
            // 
            this.PeriodicPhase.LargeChange = 100;
            this.PeriodicPhase.Location = new System.Drawing.Point(24, 110);
            this.PeriodicPhase.Maximum = 35999;
            this.PeriodicPhase.Name = "PeriodicPhase";
            this.PeriodicPhase.Size = new System.Drawing.Size(304, 45);
            this.PeriodicPhase.SmallChange = 10;
            this.PeriodicPhase.TabIndex = 4;
            this.PeriodicPhase.TickFrequency = 1000;
            this.PeriodicPhase.Scroll += new System.EventHandler(this.PeriodicScroll);
            // 
            // PeriodicOffsetLabel
            // 
            this.PeriodicOffsetLabel.AutoSize = true;
            this.PeriodicOffsetLabel.Location = new System.Drawing.Point(96, 58);
            this.PeriodicOffsetLabel.Name = "PeriodicOffsetLabel";
            this.PeriodicOffsetLabel.Size = new System.Drawing.Size(47, 13);
            this.PeriodicOffsetLabel.TabIndex = 3;
            this.PeriodicOffsetLabel.Text = "Offset: 0";
            // 
            // PeriodicOffset
            // 
            this.PeriodicOffset.LargeChange = 100;
            this.PeriodicOffset.Location = new System.Drawing.Point(24, 67);
            this.PeriodicOffset.Maximum = 10000;
            this.PeriodicOffset.Minimum = -10000;
            this.PeriodicOffset.Name = "PeriodicOffset";
            this.PeriodicOffset.Size = new System.Drawing.Size(304, 45);
            this.PeriodicOffset.SmallChange = 10;
            this.PeriodicOffset.TabIndex = 2;
            this.PeriodicOffset.TickFrequency = 1000;
            this.PeriodicOffset.Scroll += new System.EventHandler(this.PeriodicScroll);
            // 
            // PeriodicMagnitudeLabel
            // 
            this.PeriodicMagnitudeLabel.AutoSize = true;
            this.PeriodicMagnitudeLabel.Location = new System.Drawing.Point(96, 16);
            this.PeriodicMagnitudeLabel.Name = "PeriodicMagnitudeLabel";
            this.PeriodicMagnitudeLabel.Size = new System.Drawing.Size(70, 13);
            this.PeriodicMagnitudeLabel.TabIndex = 1;
            this.PeriodicMagnitudeLabel.Text = "Magnitude: 0";
            // 
            // PeriodicMagnitude
            // 
            this.PeriodicMagnitude.LargeChange = 1000;
            this.PeriodicMagnitude.Location = new System.Drawing.Point(24, 24);
            this.PeriodicMagnitude.Maximum = 10000;
            this.PeriodicMagnitude.Name = "PeriodicMagnitude";
            this.PeriodicMagnitude.Size = new System.Drawing.Size(304, 45);
            this.PeriodicMagnitude.SmallChange = 100;
            this.PeriodicMagnitude.TabIndex = 0;
            this.PeriodicMagnitude.TickFrequency = 1000;
            this.PeriodicMagnitude.Value = 5000;
            this.PeriodicMagnitude.Scroll += new System.EventHandler(this.PeriodicScroll);
            // 
            // GroupRampForce
            // 
            this.GroupRampForce.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                         this.RangeEndLabel,
                                                                                         this.RangeEnd,
                                                                                         this.RangeStartLabel,
                                                                                         this.RangeStart});
            this.GroupRampForce.Location = new System.Drawing.Point(8, 16);
            this.GroupRampForce.Name = "GroupRampForce";
            this.GroupRampForce.Size = new System.Drawing.Size(344, 200);
            this.GroupRampForce.TabIndex = 1;
            this.GroupRampForce.TabStop = false;
            this.GroupRampForce.Tag = "";
            this.GroupRampForce.Text = "Ramp Force";
            this.GroupRampForce.Visible = false;
            // 
            // RangeEndLabel
            // 
            this.RangeEndLabel.AutoSize = true;
            this.RangeEndLabel.Location = new System.Drawing.Point(93, 120);
            this.RangeEndLabel.Name = "RangeEndLabel";
            this.RangeEndLabel.Size = new System.Drawing.Size(73, 13);
            this.RangeEndLabel.TabIndex = 3;
            this.RangeEndLabel.Text = "Range End: 0";
            // 
            // RangeEnd
            // 
            this.RangeEnd.LargeChange = 100;
            this.RangeEnd.Location = new System.Drawing.Point(28, 144);
            this.RangeEnd.Maximum = 10000;
            this.RangeEnd.Minimum = -10000;
            this.RangeEnd.Name = "RangeEnd";
            this.RangeEnd.Size = new System.Drawing.Size(304, 45);
            this.RangeEnd.SmallChange = 10;
            this.RangeEnd.TabIndex = 2;
            this.RangeEnd.TickFrequency = 1000;
            this.RangeEnd.Scroll += new System.EventHandler(this.RangeScroll);
            // 
            // RangeStartLabel
            // 
            this.RangeStartLabel.AutoSize = true;
            this.RangeStartLabel.Location = new System.Drawing.Point(93, 24);
            this.RangeStartLabel.Name = "RangeStartLabel";
            this.RangeStartLabel.Size = new System.Drawing.Size(77, 13);
            this.RangeStartLabel.TabIndex = 1;
            this.RangeStartLabel.Text = "Range Start: 0";
            // 
            // RangeStart
            // 
            this.RangeStart.LargeChange = 100;
            this.RangeStart.Location = new System.Drawing.Point(28, 48);
            this.RangeStart.Maximum = 10000;
            this.RangeStart.Minimum = -10000;
            this.RangeStart.Name = "RangeStart";
            this.RangeStart.Size = new System.Drawing.Size(304, 45);
            this.RangeStart.SmallChange = 10;
            this.RangeStart.TabIndex = 0;
            this.RangeStart.TickFrequency = 1000;
            this.RangeStart.Scroll += new System.EventHandler(this.RangeScroll);
            // 
            // GroupConstantForce
            // 
            this.GroupConstantForce.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                             this.Magnitude,
                                                                                             this.ConstantForceMagnitude});
            this.GroupConstantForce.Location = new System.Drawing.Point(8, 16);
            this.GroupConstantForce.Name = "GroupConstantForce";
            this.GroupConstantForce.Size = new System.Drawing.Size(344, 200);
            this.GroupConstantForce.TabIndex = 0;
            this.GroupConstantForce.TabStop = false;
            this.GroupConstantForce.Tag = "";
            this.GroupConstantForce.Text = "Constant Force";
            this.GroupConstantForce.Visible = false;
            // 
            // Magnitude
            // 
            this.Magnitude.AutoSize = true;
            this.Magnitude.Location = new System.Drawing.Point(89, 72);
            this.Magnitude.Name = "Magnitude";
            this.Magnitude.Size = new System.Drawing.Size(175, 13);
            this.Magnitude.TabIndex = 1;
            this.Magnitude.Text = "Constant Force Magnitude: 10000";
            // 
            // ConstantForceMagnitude
            // 
            this.ConstantForceMagnitude.LargeChange = 100;
            this.ConstantForceMagnitude.Location = new System.Drawing.Point(24, 96);
            this.ConstantForceMagnitude.Maximum = 10000;
            this.ConstantForceMagnitude.Name = "ConstantForceMagnitude";
            this.ConstantForceMagnitude.Size = new System.Drawing.Size(304, 45);
            this.ConstantForceMagnitude.SmallChange = 10;
            this.ConstantForceMagnitude.TabIndex = 0;
            this.ConstantForceMagnitude.TickFrequency = 1000;
            this.ConstantForceMagnitude.Value = 10000;
            this.ConstantForceMagnitude.Scroll += new System.EventHandler(this.ConstantForceMagnitudeScroll);
            // 
            // EnvelopeGroupBox
            // 
            this.EnvelopeGroupBox.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                           this.EnvelopeFadeTime,
                                                                                           this.EnvelopeFadeTimeLabel,
                                                                                           this.EnvelopeFadeLevel,
                                                                                           this.EnvelopeFadeLevelLabel,
                                                                                           this.EnvelopeAttackTime,
                                                                                           this.EnvelopeAttackTimeLabel,
                                                                                           this.EnvelopeAttackLevel,
                                                                                           this.EnvelopeAttackLevelLabel,
                                                                                           this.chkUseEnvelope});
            this.EnvelopeGroupBox.Location = new System.Drawing.Point(246, 262);
            this.EnvelopeGroupBox.Name = "EnvelopeGroupBox";
            this.EnvelopeGroupBox.Size = new System.Drawing.Size(168, 216);
            this.EnvelopeGroupBox.TabIndex = 9;
            this.EnvelopeGroupBox.TabStop = false;
            this.EnvelopeGroupBox.Text = "Envelope";
            // 
            // EnvelopeFadeTime
            // 
            this.EnvelopeFadeTime.AutoSize = false;
            this.EnvelopeFadeTime.LargeChange = 10000;
            this.EnvelopeFadeTime.Location = new System.Drawing.Point(16, 172);
            this.EnvelopeFadeTime.Maximum = 5000000;
            this.EnvelopeFadeTime.Name = "EnvelopeFadeTime";
            this.EnvelopeFadeTime.Size = new System.Drawing.Size(144, 45);
            this.EnvelopeFadeTime.SmallChange = 1000;
            this.EnvelopeFadeTime.TabIndex = 8;
            this.EnvelopeFadeTime.TickFrequency = 1000000;
            this.EnvelopeFadeTime.Scroll += new System.EventHandler(this.EnvChanged);
            // 
            // EnvelopeFadeTimeLabel
            // 
            this.EnvelopeFadeTimeLabel.AutoSize = true;
            this.EnvelopeFadeTimeLabel.Location = new System.Drawing.Point(16, 160);
            this.EnvelopeFadeTimeLabel.Name = "EnvelopeFadeTimeLabel";
            this.EnvelopeFadeTimeLabel.Size = new System.Drawing.Size(71, 13);
            this.EnvelopeFadeTimeLabel.TabIndex = 7;
            this.EnvelopeFadeTimeLabel.Text = "Fade Time: 0";
            // 
            // EnvelopeFadeLevel
            // 
            this.EnvelopeFadeLevel.AutoSize = false;
            this.EnvelopeFadeLevel.LargeChange = 1000;
            this.EnvelopeFadeLevel.Location = new System.Drawing.Point(16, 128);
            this.EnvelopeFadeLevel.Maximum = 10000;
            this.EnvelopeFadeLevel.Name = "EnvelopeFadeLevel";
            this.EnvelopeFadeLevel.Size = new System.Drawing.Size(144, 45);
            this.EnvelopeFadeLevel.SmallChange = 100;
            this.EnvelopeFadeLevel.TabIndex = 6;
            this.EnvelopeFadeLevel.TickFrequency = 1000;
            this.EnvelopeFadeLevel.Scroll += new System.EventHandler(this.EnvChanged);
            // 
            // EnvelopeFadeLevelLabel
            // 
            this.EnvelopeFadeLevelLabel.AutoSize = true;
            this.EnvelopeFadeLevelLabel.Location = new System.Drawing.Point(16, 116);
            this.EnvelopeFadeLevelLabel.Name = "EnvelopeFadeLevelLabel";
            this.EnvelopeFadeLevelLabel.Size = new System.Drawing.Size(73, 13);
            this.EnvelopeFadeLevelLabel.TabIndex = 5;
            this.EnvelopeFadeLevelLabel.Text = "Fade Level: 0";
            // 
            // EnvelopeAttackTime
            // 
            this.EnvelopeAttackTime.AutoSize = false;
            this.EnvelopeAttackTime.LargeChange = 50000;
            this.EnvelopeAttackTime.Location = new System.Drawing.Point(16, 88);
            this.EnvelopeAttackTime.Maximum = 5000000;
            this.EnvelopeAttackTime.Name = "EnvelopeAttackTime";
            this.EnvelopeAttackTime.Size = new System.Drawing.Size(144, 45);
            this.EnvelopeAttackTime.SmallChange = 1000;
            this.EnvelopeAttackTime.TabIndex = 4;
            this.EnvelopeAttackTime.TickFrequency = 1000000;
            this.EnvelopeAttackTime.Scroll += new System.EventHandler(this.EnvChanged);
            // 
            // EnvelopeAttackTimeLabel
            // 
            this.EnvelopeAttackTimeLabel.AutoSize = true;
            this.EnvelopeAttackTimeLabel.Location = new System.Drawing.Point(16, 76);
            this.EnvelopeAttackTimeLabel.Name = "EnvelopeAttackTimeLabel";
            this.EnvelopeAttackTimeLabel.Size = new System.Drawing.Size(76, 13);
            this.EnvelopeAttackTimeLabel.TabIndex = 3;
            this.EnvelopeAttackTimeLabel.Text = "Attack Time: 0";
            // 
            // EnvelopeAttackLevel
            // 
            this.EnvelopeAttackLevel.AutoSize = false;
            this.EnvelopeAttackLevel.LargeChange = 1000;
            this.EnvelopeAttackLevel.Location = new System.Drawing.Point(16, 44);
            this.EnvelopeAttackLevel.Maximum = 10000;
            this.EnvelopeAttackLevel.Name = "EnvelopeAttackLevel";
            this.EnvelopeAttackLevel.Size = new System.Drawing.Size(144, 45);
            this.EnvelopeAttackLevel.SmallChange = 100;
            this.EnvelopeAttackLevel.TabIndex = 2;
            this.EnvelopeAttackLevel.TickFrequency = 1000;
            this.EnvelopeAttackLevel.Scroll += new System.EventHandler(this.EnvChanged);
            // 
            // EnvelopeAttackLevelLabel
            // 
            this.EnvelopeAttackLevelLabel.AutoSize = true;
            this.EnvelopeAttackLevelLabel.Location = new System.Drawing.Point(16, 32);
            this.EnvelopeAttackLevelLabel.Name = "EnvelopeAttackLevelLabel";
            this.EnvelopeAttackLevelLabel.Size = new System.Drawing.Size(78, 13);
            this.EnvelopeAttackLevelLabel.TabIndex = 1;
            this.EnvelopeAttackLevelLabel.Text = "Attack Level: 0";
            // 
            // chkUseEnvelope
            // 
            this.chkUseEnvelope.Location = new System.Drawing.Point(16, 12);
            this.chkUseEnvelope.Name = "chkUseEnvelope";
            this.chkUseEnvelope.Size = new System.Drawing.Size(96, 24);
            this.chkUseEnvelope.TabIndex = 0;
            this.chkUseEnvelope.Text = "Use Envelope";
            this.chkUseEnvelope.CheckedChanged += new System.EventHandler(this.EnvChanged);
            // 
            // DirectionGroupBox
            // 
            this.DirectionGroupBox.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                            this.NorthEast,
                                                                                            this.East,
                                                                                            this.SouthEast,
                                                                                            this.South,
                                                                                            this.SouthWest,
                                                                                            this.West,
                                                                                            this.NorthWest,
                                                                                            this.North});
            this.DirectionGroupBox.Location = new System.Drawing.Point(438, 262);
            this.DirectionGroupBox.Name = "DirectionGroupBox";
            this.DirectionGroupBox.Size = new System.Drawing.Size(168, 216);
            this.DirectionGroupBox.TabIndex = 10;
            this.DirectionGroupBox.TabStop = false;
            this.DirectionGroupBox.Text = "Direction";
            // 
            // NorthEast
            // 
            this.NorthEast.Location = new System.Drawing.Point(120, 64);
            this.NorthEast.Name = "NorthEast";
            this.NorthEast.Size = new System.Drawing.Size(16, 24);
            this.NorthEast.TabIndex = 7;
            this.NorthEast.Tag = "1,-1";
            this.NorthEast.CheckedChanged += new System.EventHandler(this.DirectionChanged);
            // 
            // East
            // 
            this.East.Checked = true;
            this.East.Location = new System.Drawing.Point(136, 104);
            this.East.Name = "East";
            this.East.Size = new System.Drawing.Size(16, 24);
            this.East.TabIndex = 6;
            this.East.TabStop = true;
            this.East.Tag = "2,0";
            this.East.CheckedChanged += new System.EventHandler(this.DirectionChanged);
            // 
            // SouthEast
            // 
            this.SouthEast.Location = new System.Drawing.Point(120, 144);
            this.SouthEast.Name = "SouthEast";
            this.SouthEast.Size = new System.Drawing.Size(16, 24);
            this.SouthEast.TabIndex = 5;
            this.SouthEast.Tag = "1,1";
            this.SouthEast.CheckedChanged += new System.EventHandler(this.DirectionChanged);
            // 
            // South
            // 
            this.South.Location = new System.Drawing.Point(80, 168);
            this.South.Name = "South";
            this.South.Size = new System.Drawing.Size(16, 24);
            this.South.TabIndex = 4;
            this.South.Tag = "0,2";
            this.South.CheckedChanged += new System.EventHandler(this.DirectionChanged);
            // 
            // SouthWest
            // 
            this.SouthWest.Location = new System.Drawing.Point(40, 144);
            this.SouthWest.Name = "SouthWest";
            this.SouthWest.Size = new System.Drawing.Size(16, 24);
            this.SouthWest.TabIndex = 3;
            this.SouthWest.Tag = "-1,1";
            this.SouthWest.CheckedChanged += new System.EventHandler(this.DirectionChanged);
            // 
            // West
            // 
            this.West.Location = new System.Drawing.Point(24, 104);
            this.West.Name = "West";
            this.West.Size = new System.Drawing.Size(16, 24);
            this.West.TabIndex = 2;
            this.West.Tag = "-2,0";
            this.West.CheckedChanged += new System.EventHandler(this.DirectionChanged);
            // 
            // NorthWest
            // 
            this.NorthWest.Location = new System.Drawing.Point(40, 64);
            this.NorthWest.Name = "NorthWest";
            this.NorthWest.Size = new System.Drawing.Size(16, 24);
            this.NorthWest.TabIndex = 1;
            this.NorthWest.Tag = "-1,-1";
            this.NorthWest.CheckedChanged += new System.EventHandler(this.DirectionChanged);
            // 
            // North
            // 
            this.North.Location = new System.Drawing.Point(80, 40);
            this.North.Name = "North";
            this.North.Size = new System.Drawing.Size(16, 24);
            this.North.TabIndex = 0;
            this.North.Tag = "0,-2";
            this.North.CheckedChanged += new System.EventHandler(this.DirectionChanged);
            // 
            // frmMain
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(613, 485);
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.Label1,
                                                                          this.gbGeneralParams,
                                                                          this.lstEffects,
                                                                          this.gbTypeContainer,
                                                                          this.EnvelopeGroupBox,
                                                                          this.DirectionGroupBox});
            this.Name = "frmMain";
            this.Text = "Feedback";
            this.Closing += new System.ComponentModel.CancelEventHandler(this.frmMain_Closing);
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.Activated += new System.EventHandler(this.frmMain_Activated);
            this.gbGeneralParams.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.GeneralPeriod)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.GeneralGain)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.GeneralDuration)).EndInit();
            this.gbTypeContainer.ResumeLayout(false);
            this.GroupConditionalForce.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalPositiveSaturation)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalNegativeSaturation)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalPositiveCoefficient)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalNegativeCoeffcient)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalOffset)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ConditionalDeadBand)).EndInit();
            this.GroupPeriodForce.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.PeriodicPeriod)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PeriodicPhase)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PeriodicOffset)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PeriodicMagnitude)).EndInit();
            this.GroupRampForce.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.RangeEnd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RangeStart)).EndInit();
            this.GroupConstantForce.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.ConstantForceMagnitude)).EndInit();
            this.EnvelopeGroupBox.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.EnvelopeFadeTime)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EnvelopeFadeLevel)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EnvelopeAttackTime)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EnvelopeAttackLevel)).EndInit();
            this.DirectionGroupBox.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        
        
        
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main() 
        {            
            frmMain main = new frmMain();
            if (!main.IsDisposed)
                Application.Run(main);
        }

        
        
        
        /// <summary>
        /// Handles the changing of an effect.
        /// </summary>
        private void lstEffects_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            EffectDescription description;

            if (null != effectSelected)
                effectSelected.Unload();

            description = (EffectDescription)lstEffects.Items[lstEffects.SelectedIndex];
            effectSelected = description.effectSelected;
            UpdateVisibilty();
            
            try
            {
                effectSelected.Start(1);
            }
            catch(InputException){}
        }
        
        
        
        
        /// <summary>
        /// Changes the parameters of an effect.
        /// </summary>
        private Effect ChangeParameter()
        {
            Effect eff = GetEffectParameters();
            int flags = (int)EffectParameterFlags.Start;
            int i = 0;

            switch (eff.EffectType)
            {
                case EffectType.ConstantForce:
                    eff.Constant.Magnitude = ConstantForceMagnitude.Value;
                    flags = flags | (int)EffectParameterFlags.TypeSpecificParams;
                    break;
                case EffectType.RampForce:
                    eff.RampStruct.Start= RangeStart.Value;
                    eff.RampStruct.End  = RangeEnd.Value;
                    flags = (int)EffectParameterFlags.TypeSpecificParams;
                    if ((int)DI.Infinite == eff.Duration)
                    {
                        // Default to a 2 second ramp effect
                        // if DI.Infinite is passed in.
                        // DI.Infinite is invalid for ramp forces.
                        eff.Duration = 2 * (int)DI.Seconds;
                        flags = flags | (int)EffectParameterFlags.Duration;
                    }
                    flags = flags | (int)EffectParameterFlags.Start;
                    break;
                case EffectType.Periodic:
                        
                    eff.Periodic.Magnitude= PeriodicMagnitude.Value;
                    eff.Periodic.Offset = PeriodicOffset.Value;
                    eff.Periodic.Period = PeriodicPeriod.Value;
                    eff.Periodic.Phase  = PeriodicPhase.Value;
                        
                    flags = flags | (int)EffectParameterFlags.TypeSpecificParams;
                    break;
                case EffectType.Condition:
                    if (ConditionalAxis1.Checked == true)
                        i = 0;
                    else                                                                    
                        i = 1;          
                
                    eff.ConditionStruct[i].DeadBand             = ConditionalDeadBand.Value;
                    eff.ConditionStruct[i].NegativeCoefficient  = ConditionalNegativeCoeffcient.Value;
                    eff.ConditionStruct[i].NegativeSaturation   = ConditionalNegativeSaturation.Value;
                    eff.ConditionStruct[i].Offset               = ConditionalOffset.Value;
                    eff.ConditionStruct[i].PositiveCoefficient  = ConditionalPositiveCoefficient.Value;
                    eff.ConditionStruct[i].PositiveSaturation   = ConditionalPositiveSaturation.Value;
                                                                                                                                                            
                    flags = flags | (int)EffectParameterFlags.TypeSpecificParams;
                    break;
            }
            
            // Some feedback drivers will fail when setting parameters that aren't supported by
            // an effect. DirectInput will will in turn pass back the driver error to the application.
            // Since these are hardware specific error messages that can't be handled individually, 
            // the app will ignore any failures returned to SetParameters().
            try
            {
                effectSelected.SetParameters(eff, EffectParameterFlags.TypeSpecificParams);
            }
            catch
            {
                eff = GetEffectParameters();
            }

            return eff;
        }
        
        
        
        
        /// <summary>
        /// Changes the direction of an effect.
        /// </summary>
        private void ChangeDirection(int[] direction)
        {
            Effect eff = new Effect();
            
            eff.Flags = EffectFlags.Cartesian | EffectFlags.ObjectOffsets;
            effectSelected.GetParameters(ref eff, EffectParameterFlags.AllParams);
            eff.SetDirection(direction);
            
            // Some feedback drivers will fail when setting parameters that aren't supported by
            // an effect. DirectInput will will in turn pass back the driver error to the application.
            // Since these are hardware specific error messages that can't be handled individually, 
            // the app will ignore any failures returned to SetParameters().
            try
            {
                effectSelected.SetParameters(eff, EffectParameterFlags.Direction | EffectParameterFlags.Start);
            }
            catch(InputException){}
        }
        
        
        
        
        /// <summary>
        /// Changes the envelope of an effect.
        /// </summary>
        private Effect ChangeEnvelope()
        {
            Effect eff = GetEffectParameters();

            if (!isChanging)
            {
                eff.UsesEnvelope = chkUseEnvelope.Checked;
                eff.EnvelopeStruct.AttackLevel = EnvelopeAttackLevel.Value;
                eff.EnvelopeStruct.AttackTime = EnvelopeAttackTime.Value;
                eff.EnvelopeStruct.FadeLevel = EnvelopeFadeLevel.Value;
                eff.EnvelopeStruct.FadeTime = EnvelopeFadeTime.Value;

                // Some feedback drivers will fail when setting parameters that aren't supported by
                // an effect. DirectInput will will in turn pass back the driver error to the application.
                // Since these are hardware specific error messages that can't be handled individually, 
                // the app will ignore any failures returned to SetParameters().
                try
                {
                    effectSelected.SetParameters(eff, EffectParameterFlags.Envelope | EffectParameterFlags.Start);
                }
                catch(InputException){}
            }
            return eff;
        }
        
        
        
        
        /// <summary>
        /// Fills in an Effect structure with effect information.
        /// </summary>
        private Effect GetEffectParameters()
        {
            Effect eff = new Effect();

            eff.Flags = EffectFlags.ObjectIds | EffectFlags.Cartesian;
            effectSelected.GetParameters(ref eff, EffectParameterFlags.AllParams);

            // If this is a condition effect, see if the Effect.Condition member
            // array length that was returned from GetParameters() has enough elements 
            // to cover 2 axes if this is a two axis device. In most cases, conditional 
            // effects will return 1 Condition element that can be applied across 
            // all force-feedback axes.
            if ((eff.EffectType == EffectType.Condition) && (eff.ConditionStruct != null))
                                                        {
                if ((rbConditionalAxis2.Enabled) && (eff.ConditionStruct.Length < 2))
                {
                    // Resize the array for two axes.
                    Condition[] temp = new Condition[2];
                    eff.ConditionStruct.CopyTo(temp,0);
                    eff.ConditionStruct = temp;
                    // Copy the conditional effect info from one struct to the other.
                    eff.ConditionStruct[1] = eff.ConditionStruct[0];
                }
            }
            return eff;
        }
        
        
        
        
        /// <summary>
        /// Updates the visibility of each
        /// effect parameters group box, as well
        /// as general parameter, envelope, and
        /// direction group boxes.
        /// </summary>
        private void UpdateVisibilty()
        {
            isChanging = true;

            if (null == effectSelected) 
                return;

            EffectDescription description = (EffectDescription)lstEffects.Items[lstEffects.SelectedIndex];
            Effect eff = GetEffectParameters();

            GroupBox Current = new GroupBox();

            // Check to see what type of effect this is,
            // and then change the visibilty of the
            // group boxes accordingly.
            switch (DInputHelper.GetTypeCode((int)eff.EffectType))
            {
                case (int)EffectType.Condition:
                    Current = GroupConditionalForce;
                    UpdateConditionalGroupBox(eff);
                    break;
                case (int)EffectType.ConstantForce:
                    Current = GroupConstantForce;
                    UpdateConstantGroupBox(eff);
                    break;
                case (int)EffectType.Periodic:
                    Current = GroupPeriodForce;
                    UpdatePeriodicGroupBox(eff);
                    break;
                case (int)EffectType.RampForce:
                    Current = GroupRampForce;
                    UpdateRampGroupBox(eff);
                    break;
            }

            foreach (GroupBox target in gbTypeContainer.Controls)
            {
                if (Current == target)
                    target.Visible = true;
                else
                    target.Visible = false;
            }

            // Check the effect info and update the controls
            // to show whether the parameters are supported.
            if (0 == (description.info.StaticParams & (int)EffectParameterFlags.Direction))
                DirectionGroupBox.Enabled = false;
            else
                DirectionGroupBox.Enabled = true;

            if (0 == (description.info.StaticParams & (int)EffectParameterFlags.Duration))
                GeneralDurationLabel.Enabled = GeneralDuration.Enabled = false;
            else
                GeneralDurationLabel.Enabled = GeneralDuration.Enabled = true;

            if (0 == (description.info.StaticParams & (int)EffectParameterFlags.Gain))
                GeneralGainLabel.Enabled = GeneralGain.Enabled = false;
            else
                GeneralGainLabel.Enabled = GeneralGain.Enabled = true;

            if (0 == (description.info.StaticParams & (int)EffectParameterFlags.SamplePeriod))
                GeneralPeriodLabel.Enabled = GeneralPeriod.Enabled = false;
            else
                GeneralPeriodLabel.Enabled = GeneralPeriod.Enabled = true;

            // Update the general parameter
            // and envelope controls.
            UpdateGeneralParamsGroupBox(eff);

            // Reflect support for envelopes on this effect.
            UpdateEnvParamsGroupBox(eff);
            EnvelopeGroupBox.Enabled = ((description.info.StaticParams & (int)EffectParameterFlags.Envelope) != 0) ? true : false;
 
            // Update direction radio buttons.
            if (1 == axis.Length)
            {
                if (2 == eff.GetDirection()[0])
                    East.Checked = true;
                else
                    West.Checked = true;
            }
            else if (2 >= axis.Length)
            {
                if (2 == eff.GetDirection()[0] && 0 == eff.GetDirection()[1])
                    East.Checked = true;
                else if (-2 == eff.GetDirection()[0] && 0 == eff.GetDirection()[1])
                    West.Checked = true;
                else if (0 == eff.GetDirection()[0] && -2 == eff.GetDirection()[1])
                    North.Checked = true;
                else if (0 == eff.GetDirection()[0] && 2 == eff.GetDirection()[1])
                    South.Checked = true;
                else if (1 == eff.GetDirection()[0] && -1 == eff.GetDirection()[1])
                    NorthEast.Checked = true;
                else if (1 == eff.GetDirection()[0] && 1 == eff.GetDirection()[1])
                    SouthEast.Checked = true;
                else if (-1 == eff.GetDirection()[0] && 1 == eff.GetDirection()[1])
                    SouthWest.Checked = true;
                else if (-1 == eff.GetDirection()[0] && -1 == eff.GetDirection()[1])
                    NorthWest.Checked = true;
                else if (0 == eff.GetDirection()[0] && 0 == eff.GetDirection()[1])
                    East.Checked = true;
            }

            isChanging = false;
        }
        
        
        
        
        /// <summary>
        /// Updates the general parameters controls and labels.
        /// </summary>
        private void UpdateGeneralParamsGroupBox(Effect eff)
        {
            if (((eff.Duration / (int)DI.Seconds) > GeneralDuration.Maximum) || (eff.Duration <  0))
                GeneralDuration.Value = GeneralDuration.Maximum;
            else
                GeneralDuration.Value = eff.Duration / (int)DI.Seconds;

            if (eff.Gain > GeneralGain.Maximum)
                GeneralGain.Value = GeneralGain.Maximum;
            else
                GeneralGain.Value = eff.Gain;
            
            if (eff.SamplePeriod > GeneralPeriod.Maximum)
                GeneralPeriod.Value = GeneralPeriod.Maximum;
            else
                GeneralPeriod.Value = eff.SamplePeriod;
                     
            if ((int)DI.Infinite == eff.Duration)
                GeneralDurationLabel.Text = "Effect Duration: Infinite";
            else
                GeneralDurationLabel.Text = "Effect Duration: " + (eff.Duration / (int)DI.Seconds) + " seconds";
            
            GeneralGainLabel.Text = "Effect Gain: " + GeneralGain.Value;

            if (0 == eff.SamplePeriod)
                GeneralPeriodLabel.Text = "Sample Rate: Default";
            else 
                GeneralPeriodLabel.Text = "Sample Period: " + eff.SamplePeriod;

        }
        
        
        
        
        /// <summary>
        /// Updates the controls and labels for constant force effects.
        /// </summary>
        private void UpdateConstantGroupBox(Effect eff)
        {
            ConstantForceMagnitude.Value = eff.Constant.Magnitude;
            Magnitude.Text = "Constant Force Magnitude: " + ConstantForceMagnitude.Value;
        }
        
        
        
        
        /// <summary>
        /// Updates the controls and labels for ramp effects.
        /// </summary>
        private void UpdateRampGroupBox(Effect eff)
        {
            RangeStart.Value = eff.RampStruct.Start;
            RangeEnd.Value = eff.RampStruct.End; 
            RangeStartLabel.Text = "Range Start: " + RangeStart.Value;
            RangeEndLabel.Text = "Range End: " + RangeEnd.Value;
        }
        
        
        
        
        /// <summary>
        /// Updates the controls and labels for periodic effects.
        /// </summary>
        private void UpdatePeriodicGroupBox(Effect eff)
        {
            if (eff.Periodic.Magnitude < PeriodicMagnitude.Maximum)
                PeriodicMagnitude.Value = eff.Periodic.Magnitude;
            else
                PeriodicMagnitude.Value = PeriodicMagnitude.Maximum;

            if (eff.Periodic.Offset < PeriodicOffset.Maximum)
                PeriodicOffset.Value = eff.Periodic.Offset;
            else
                PeriodicOffset.Value = PeriodicOffset.Maximum;
            
            if (eff.Periodic.Period < PeriodicPeriod.Maximum)
                PeriodicPeriod.Value = eff.Periodic.Period;
            else
                PeriodicPeriod.Value = PeriodicPeriod.Maximum;
            
            if (eff.Periodic.Phase < PeriodicPhase.Maximum)
                PeriodicPhase.Value = eff.Periodic.Phase;
            else
                PeriodicPhase.Value = PeriodicPhase.Maximum;
            
            PeriodicMagnitudeLabel.Text = "Magnitude: " + PeriodicMagnitude.Value;
            PeriodicOffsetLabel.Text = "Offset: " + PeriodicOffset.Value;
            PeriodicPeriodLabel.Text = "Period: " + PeriodicPeriod.Value;
            PeriodicPhaseLabel.Text = "Phase: " + PeriodicPhase.Value;
        }
        
        
        
        
        /// <summary>
        /// Updates the controls in the Conditional group box.
        /// </summary>
        private void UpdateConditionalGroupBox(Effect eff)
        {
            int i;

            if (true == ConditionalAxis1.Checked)
                i = 0;
            else
                i = 1;
            
            ConditionalDeadBand.Value = eff.ConditionStruct[i].DeadBand;
            ConditionalOffset.Value = eff.ConditionStruct[i].Offset;
            ConditionalNegativeCoeffcient.Value = eff.ConditionStruct[i].NegativeCoefficient;
            ConditionalNegativeSaturation.Value = eff.ConditionStruct[i].NegativeSaturation;
            ConditionalPositiveCoefficient.Value = eff.ConditionStruct[i].PositiveCoefficient;
            ConditionalPositiveSaturation.Value = eff.ConditionStruct[i].PositiveSaturation;

            ConditionalDeadBandLabel.Text = "Dead Band: " + ConditionalDeadBand.Value;
            ConditionalOffsetLabel.Text = "Offset: " + ConditionalOffset.Value;
            ConditionalNegativeCoeffcientLabel.Text = "Negative Coefficient: " + ConditionalNegativeCoeffcient.Value;
            ConditionalNegativeSaturationLabel.Text = "Negative Saturation: " + ConditionalNegativeSaturation.Value;
            ConditionalPositiveCoefficientLabel.Text = "Positive Coefficient: " + ConditionalPositiveCoefficient.Value;
            ConditionalPositiveSaturationLabel.Text = "Positive Saturation: " + ConditionalPositiveSaturation.Value;            

        }
        
        
        
        
        /// <summary>
        /// Updates the env params group box
        /// </summary>
        private void UpdateEnvParamsGroupBox(Effect eff)
        {
            chkUseEnvelope.Checked = (eff.UsesEnvelope) ? true: false;

            if (eff.EnvelopeStruct.AttackLevel > EnvelopeAttackLevel.Maximum)
                EnvelopeAttackLevel.Value = EnvelopeAttackLevel.Maximum;
            else
                EnvelopeAttackLevel.Value = eff.EnvelopeStruct.AttackLevel;
            
            if (eff.EnvelopeStruct.AttackTime > EnvelopeAttackTime.Maximum)
                EnvelopeAttackTime.Value = EnvelopeAttackTime.Maximum;
            else
                EnvelopeAttackTime.Value = eff.EnvelopeStruct.AttackTime;
            
            if (eff.EnvelopeStruct.FadeLevel > EnvelopeFadeLevel.Maximum)
                EnvelopeFadeLevel.Value = EnvelopeFadeLevel.Maximum;
            else
                EnvelopeFadeLevel.Value = eff.EnvelopeStruct.FadeLevel;
            
            if (eff.EnvelopeStruct.FadeTime > EnvelopeFadeTime.Maximum)
                EnvelopeFadeTime.Value = EnvelopeFadeTime.Maximum;
            else
                EnvelopeFadeTime.Value = eff.EnvelopeStruct.FadeTime;
            
            EnvelopeAttackLevelLabel.Text = "Attack Level: " + eff.EnvelopeStruct.AttackLevel;
            EnvelopeAttackTimeLabel.Text = "Attack Time: " + eff.EnvelopeStruct.AttackTime / 1000;
            EnvelopeFadeLevelLabel.Text = "Fade Level: " + eff.EnvelopeStruct.FadeLevel;
            EnvelopeFadeTimeLabel.Text = "Fade Time: " + eff.EnvelopeStruct.FadeTime / 1000;
        }

        
        
        
        /// <summary>
        /// Handles changing the axis on a conditional effect.
        /// </summary>
        private void ConditionalAxisChanged(object sender, System.EventArgs e)
        {
            Effect eff = GetEffectParameters();
            UpdateConditionalGroupBox(eff);
        
        }

        
        
        
        /// <summary>
        /// Handles the trackbar scroll events for constant effects.
        /// </summary>
        private void ConstantForceMagnitudeScroll(object sender, System.EventArgs e)
        {
            Effect eff = ChangeParameter();
            UpdateConstantGroupBox(eff);
        
        }

        
        
        
        /// <summary>
        /// Handles the trackbar scroll events for ramp effects.
        /// </summary>
        private void RangeScroll(object sender, System.EventArgs e)
        {
            Effect eff = ChangeParameter();
            UpdateRampGroupBox(eff);
        
        }

        
        
        
        /// <summary>
        /// Handles the trackbar scroll events for periodic effects.
        /// </summary>
        private void PeriodicScroll(object sender, System.EventArgs e)
        {
            Effect eff = ChangeParameter();
            UpdatePeriodicGroupBox(eff);
        
        }

        
        
        
        /// <summary>
        /// Handles the trackbar scroll events for conditional effects.
        /// </summary>
        private void ConditionalScroll(object sender, System.EventArgs e)
        {
            Effect eff = new Effect();

            if (1 <= axis.Length)
                rbConditionalAxis2.Enabled = true;
            else
                rbConditionalAxis2.Enabled = false;
            
            eff = ChangeParameter();
            UpdateConditionalGroupBox(eff);
        
        }

        
        
        
        /// <summary>
        /// Handles direction changes.
        /// </summary>
        private void DirectionChanged(object sender, System.EventArgs e)
        {
            int[] direction = new int[2];
            string[] values;

            foreach (RadioButton rb in DirectionGroupBox.Controls)
            {
                if (rb.Checked)
                {
                    values = rb.Tag.ToString().Split(',');
                    direction[0] = Convert.ToInt32(values[0]);
                    direction[1] = Convert.ToInt32(values[1]);
                    ChangeDirection(direction);
                    return;
                }
            }
        }

        
        
        
        /// <summary>
        /// Handles general parameter changes.
        /// </summary>
        private void GenScroll(object sender, System.EventArgs e)
        {
            Effect eff = GetEffectParameters();

            if (GeneralDuration.Value == GeneralDuration.Maximum)
                eff.Duration = (int)DI.Infinite;
            else
                eff.Duration = GeneralDuration.Value * (int)DI.Seconds;
            
            eff.Gain = GeneralGain.Value;
            eff.SamplePeriod = GeneralPeriod.Value;

            UpdateGeneralParamsGroupBox(eff);
            
            // Some feedback drivers will fail when setting parameters that aren't supported by
            // an effect. DirectInput will will in turn pass back the driver error to the application.
            // Since these are hardware specific error messages that can't be handled individually, 
            // the app will ignore any failures returned to SetParameters().
            try
            {
                effectSelected.SetParameters(eff, EffectParameterFlags.Duration | EffectParameterFlags.Gain | EffectParameterFlags.SamplePeriod | EffectParameterFlags.Start);
            }
            catch(DirectXException){}       
        }

        
        
        
        private void EnvChanged(object sender, System.EventArgs e)
        {
            Effect eff = new Effect();

            eff = ChangeEnvelope();
            UpdateEnvParamsGroupBox(eff);
        }

        
        
        
        private void frmMain_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            DestroyObjects();
        }

        
        
        
        private void frmMain_Load(object sender, System.EventArgs e)
        {
            //Initialize the DirectInput objects
            if (!InitializeDirectInput())
                Close();
        }
        
        
        
        
        /// <summary>
        /// Destroys all objects.
        /// </summary>
        private void DestroyObjects()
        {
            if (null == applicationDevice)
                return;

            try
            {
                if (null != effectSelected)
                    effectSelected.Stop();
            
                foreach (EffectDescription description in lstEffects.Items)
                {
                    if (null != description.effectSelected)
                        description.effectSelected.Dispose();
                }

                applicationDevice.Unacquire();
                applicationDevice.Properties.AutoCenter = true;

            }
            catch(InputException){}
        }

        private void frmMain_Activated(object sender, System.EventArgs e)
        {
            try
            {
                // Aquire the device
                applicationDevice.Acquire();
            }
            catch(InputException){}

            lstEffects_SelectedIndexChanged(null, EventArgs.Empty);
        }
    }
}
